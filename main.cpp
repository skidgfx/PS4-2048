#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sstream>
#include <pthread.h>
#include <orbis/AudioOut.h>
#include <orbis/Sysmodule.h>
#include "graphics.h"
#include "controller.h"

#define GRID_SIZE 4
#define WIN_TILE 2048
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

// Tile dimensions and positions
#define TILE_SIZE 150
#define TILE_PADDING 20
#define GRID_START_X 560    
#define GRID_START_Y 240

// Audio defines
#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 1024

// WAV file header structure
struct WavHeader {
    char riff[4];           // "RIFF"
    uint32_t fileSize;
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];           // "data"
    uint32_t dataSize;
};

// Game states
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER
};

// Logging (required by graphics.cpp)
std::stringstream debugLogStream;

// Game state
int grid[GRID_SIZE][GRID_SIZE]; 
int score = 0;
int highScore = 0;
bool gameOver = false;
bool hasWon = false;
int frameID = 0;
GameState currentState = STATE_MENU;
int menuSelection = 0;

// Audio state
int audioHandle = -1;
bool audioInitialized = false;
bool audioThreadRunning = false;
pthread_t audioThread;
int16_t* audioData = nullptr;
size_t audioDataSize = 0;
size_t audioCurrentPos = 0;

// Color definitions
Color bgColor = { 0xFA, 0xF8, 0xEF, 0xFF };
Color emptyTileColor = { 0xCD, 0xC1, 0xB4, 0xFF };
Color tile2Color = { 0xEE, 0xE4, 0xDA, 0xFF };
Color tile4Color = { 0xED, 0xE0, 0xC8, 0xFF };
Color tile8Color = { 0xF2, 0xB1, 0x79, 0xFF };
Color tile16Color = { 0xF5, 0x95, 0x63, 0xFF };
Color tile32Color = { 0xF6, 0x7C, 0x5F, 0xFF };
Color tile64Color = { 0xF6, 0x5E, 0x3B, 0xFF };
Color tile128Color = { 0xED, 0xCF, 0x72, 0xFF };
Color tile256Color = { 0xED, 0xCC, 0x61, 0xFF };
Color tile512Color = { 0xED, 0xC8, 0x50, 0xFF };
Color tile1024Color = { 0xED, 0xC5, 0x3F, 0xFF };
Color tile2048Color = { 0xED, 0xC2, 0x2E, 0xFF };
Color darkTextColor = { 0x77, 0x6E, 0x65, 0xFF };
Color lightTextColor = { 0xF9, 0xF6, 0xF2, 0xFF };
Color menuHighlightColor = { 0xF6, 0x7C, 0x5F, 0xFF };

// Simple 5x7 bitmap font for digits 0-9
const uint8_t digitBitmaps[10][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x1F}, // 0
    {0x08, 0x18, 0x08, 0x08, 0x1C}, // 1
    {0x1F, 0x01, 0x1F, 0x10, 0x1F}, // 2
    {0x1F, 0x01, 0x0F, 0x01, 0x1F}, // 3
    {0x11, 0x11, 0x1F, 0x01, 0x01}, // 4
    {0x1F, 0x10, 0x1F, 0x01, 0x1F}, // 5
    {0x1F, 0x10, 0x1F, 0x11, 0x1F}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08}, // 7
    {0x1F, 0x11, 0x1F, 0x11, 0x1F}, // 8
    {0x1F, 0x11, 0x1F, 0x01, 0x1F}  // 9
};

// Simple 5x7 bitmap font for letters (uppercase A-Z)
const uint8_t letterBitmaps[26][7] = {
    {0x0E, 0x11, 0x1F, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x1E, 0x11, 0x1E}, // B
    {0x0F, 0x10, 0x10, 0x10, 0x0F}, // C
    {0x1E, 0x11, 0x11, 0x11, 0x1E}, // D
    {0x1F, 0x10, 0x1E, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x1E, 0x10, 0x10}, // F
    {0x0F, 0x10, 0x17, 0x11, 0x0F}, // G
    {0x11, 0x11, 0x1F, 0x11, 0x11}, // H
    {0x0E, 0x04, 0x04, 0x04, 0x0E}, // I
    {0x01, 0x01, 0x01, 0x11, 0x0E}, // J
    {0x11, 0x12, 0x1C, 0x12, 0x11}, // K
    {0x10, 0x10, 0x10, 0x10, 0x1F}, // L
    {0x11, 0x1B, 0x15, 0x11, 0x11}, // M
    {0x11, 0x19, 0x15, 0x13, 0x11}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1E, 0x11, 0x1E, 0x10, 0x10}, // P
    {0x0E, 0x11, 0x15, 0x12, 0x0D}, // Q
    {0x1E, 0x11, 0x1E, 0x12, 0x11}, // R
    {0x0F, 0x10, 0x0E, 0x01, 0x1E}, // S
    {0x1F, 0x04, 0x04, 0x04, 0x04}, // T
    {0x11, 0x11, 0x11, 0x11, 0x0E}, // U
    {0x11, 0x11, 0x11, 0x0A, 0x04}, // V
    {0x11, 0x11, 0x15, 0x1B, 0x11}, // W
    {0x11, 0x0A, 0x04, 0x0A, 0x11}, // X
    {0x11, 0x0A, 0x04, 0x04, 0x04}, // Y
    {0x1F, 0x02, 0x04, 0x08, 0x1F}  // Z
};

// Load WAV file
bool loadWavFile(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open WAV file: %s\n", filename);
        return false;
    }
    
    // Read WAV header
    WavHeader header;
    fread(&header, sizeof(WavHeader), 1, file);
    
    // Validate WAV file
    if (memcmp(header.riff, "RIFF", 4) != 0 || memcmp(header.wave, "WAVE", 4) != 0) {
        printf("Invalid WAV file format\n");
        fclose(file);
        return false;
    }
    
    printf("WAV Info: %d Hz, %d channels, %d bits\n", 
           header.sampleRate, header.numChannels, header.bitsPerSample);
    
    // Allocate memory for audio data
    audioDataSize = header.dataSize;
    audioData = (int16_t*)malloc(audioDataSize);
    
    if (!audioData) {
        printf("Failed to allocate audio memory\n");
        fclose(file);
        return false;
    }
    
    // Read audio data
    fread(audioData, 1, audioDataSize, file);
    fclose(file);
    
    printf("Loaded %zu bytes of audio data\n", audioDataSize);
    return true;
}

// Audio playback thread
void* audioPlaybackThread(void* arg) {
    int16_t buffer[AUDIO_BUFFER_SIZE * AUDIO_CHANNELS];
    
    while (audioThreadRunning) {
        if (!audioData || audioDataSize == 0) {
            sceKernelUsleep(10000); // 10ms
            continue;
        }
        
        // Fill buffer with audio data
        size_t samplesNeeded = AUDIO_BUFFER_SIZE * AUDIO_CHANNELS;
        size_t samplesAvailable = (audioDataSize - audioCurrentPos) / sizeof(int16_t);
        
        if (samplesAvailable >= samplesNeeded) {
            // Copy samples to buffer
            memcpy(buffer, &audioData[audioCurrentPos / sizeof(int16_t)], samplesNeeded * sizeof(int16_t));
            audioCurrentPos += samplesNeeded * sizeof(int16_t);
        } else {
            // Loop back to beginning
            size_t firstPart = samplesAvailable;
            size_t secondPart = samplesNeeded - samplesAvailable;
            
            memcpy(buffer, &audioData[audioCurrentPos / sizeof(int16_t)], firstPart * sizeof(int16_t));
            memcpy(&buffer[firstPart], audioData, secondPart * sizeof(int16_t));
            
            audioCurrentPos = secondPart * sizeof(int16_t);
        }
        
        // Output audio
        sceAudioOutOutput(audioHandle, buffer);
    }
    
    return nullptr;
}

// Initialize audio system
bool initAudio() {
    // Load audio out module
    int ret = sceSysmoduleLoadModule((OrbisSysModule)ORBIS_SYSMODULE_INTERNAL_AUDIOOUT);
    if (ret != 0) {
        printf("Failed to load audio module: 0x%08X\n", ret);
        return false;
    }
    
    // Open audio port
    audioHandle = sceAudioOutOpen(ORBIS_USER_SERVICE_USER_ID_SYSTEM, 
                                   ORBIS_AUDIO_OUT_PORT_TYPE_MAIN, 
                                   0, 
                                   AUDIO_BUFFER_SIZE, 
                                   AUDIO_SAMPLE_RATE, 
                                   ORBIS_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
    
    if (audioHandle < 0) {
        printf("Failed to open audio port: 0x%08X\n", audioHandle);
        return false;
    }
    
    // Load WAV file
    if (!loadWavFile("/app0/assets/audio/bg.wav")) {
        printf("Failed to load background music\n");
        sceAudioOutClose(audioHandle);
        return false;
    }
    
    // Start audio playback thread
    audioThreadRunning = true;
    if (pthread_create(&audioThread, nullptr, audioPlaybackThread, nullptr) != 0) {
        printf("Failed to create audio thread\n");
        free(audioData);
        sceAudioOutClose(audioHandle);
        return false;
    }
    
    printf("Audio initialized successfully\n");
    audioInitialized = true;
    return true;
}

// Close audio system
void closeAudio() {
    if (audioThreadRunning) {
        audioThreadRunning = false;
        pthread_join(audioThread, nullptr);
    }
    
    if (audioData) {
        free(audioData);
        audioData = nullptr;
    }
    
    if (audioHandle >= 0) {
        sceAudioOutClose(audioHandle);
        audioHandle = -1;
    }
    
    audioInitialized = false;
}

// Draw a single character
void drawChar(Scene2D* scene, char c, int x, int y, Color color, int scale) {
    if (c >= '0' && c <= '9') {
        int digit = c - '0';
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (digitBitmaps[digit][row] & (1 << (4 - col))) {
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            scene->DrawPixel(x + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
    } else if (c >= 'A' && c <= 'Z') {
        int letter = c - 'A';
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (letterBitmaps[letter][row] & (1 << (4 - col))) {
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            scene->DrawPixel(x + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
    } else if (c >= 'a' && c <= 'z') {
        drawChar(scene, c - 32, x, y, color, scale);
    }
}

// Draw text string
void drawText(Scene2D* scene, const char* text, int x, int y, Color color, int scale) {
    int xOffset = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        drawChar(scene, text[i], x + xOffset, y, color, scale);
        xOffset += (6 * scale);
    }
}

// Draw a single digit
void drawDigit(Scene2D* scene, int digit, int x, int y, Color color, int scale) {
    if (digit < 0 || digit > 9) return;
    
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if (digitBitmaps[digit][row] & (1 << (4 - col))) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        scene->DrawPixel(x + col * scale + sx, y + row * scale + sy, color);
                    }
                }
            }
        }
    }
}

// Draw a number
void drawNumber(Scene2D* scene, int number, int x, int y, Color color, int scale) {
    if (number == 0) {
        drawDigit(scene, 0, x, y, color, scale);
        return;
    }
    
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", number);
    int len = strlen(buffer);
    
    int totalWidth = len * (5 * scale + scale);
    int startX = x - totalWidth / 2;
    
    for (int i = 0; i < len; i++) {
        int digit = buffer[i] - '0';
        drawDigit(scene, digit, startX + i * (5 * scale + scale), y, color, scale);
    }
}

// Initialize the grid
void initGrid() {
    memset(grid, 0, sizeof(grid));
    score = 0;
    gameOver = false;
    hasWon = false;
}

// Add a random tile (2 or 4) to an empty cell
bool addRandomTile() {
    int emptyCells[GRID_SIZE * GRID_SIZE][2];
    int emptyCount = 0;
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                emptyCells[emptyCount][0] = i;
                emptyCells[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }
    
    if (emptyCount == 0) return false;
    
    int index = rand() % emptyCount;
    int value = (rand() % 10 < 9) ? 2 : 4;
    grid[emptyCells[index][0]][emptyCells[index][1]] = value;
    return true;
}

// Get color for tile value
Color getTileColor(int value) {
    switch(value) {
        case 0: return emptyTileColor;
        case 2: return tile2Color;
        case 4: return tile4Color;
        case 8: return tile8Color;
        case 16: return tile16Color;
        case 32: return tile32Color;
        case 64: return tile64Color;
        case 128: return tile128Color;
        case 256: return tile256Color;
        case 512: return tile512Color;
        case 1024: return tile1024Color;
        case 2048: return tile2048Color;
        default: return tile2048Color;
    }
}

// Get text color based on tile value
Color getTextColor(int value) {
    if (value >= 8) {
        return lightTextColor;
    }
    return darkTextColor;
}

// Get appropriate scale for number based on value
int getNumberScale(int value) {
    if (value >= 1000) return 3;
    if (value >= 100) return 4;
    return 5;
}

// Draw a tile with number
void drawTile(Scene2D* scene, int row, int col, int value) {
    int x = GRID_START_X + col * (TILE_SIZE + TILE_PADDING);
    int y = GRID_START_Y + row * (TILE_SIZE + TILE_PADDING);
    
    Color tileColor = getTileColor(value);
    scene->DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, tileColor);
    
    if (value > 0) {
        Color textColor = getTextColor(value);
        int scale = getNumberScale(value);
        int centerX = x + TILE_SIZE / 2;
        int centerY = y + TILE_SIZE / 2 - (5 * scale) / 2;
        drawNumber(scene, value, centerX, centerY, textColor, scale);
    }
}

// Draw main menu
void drawMenu(Scene2D* scene) {
    scene->FrameBufferFill(bgColor);
    
    drawNumber(scene, 2048, 960, 200, darkTextColor, 16);
    
    Color startColor = (menuSelection == 0) ? menuHighlightColor : darkTextColor;
    drawText(scene, "START GAME", 760, 450, startColor, 6);
    
    drawText(scene, "HIGH SCORE", 740, 600, darkTextColor, 5);
    drawNumber(scene, highScore, 960, 670, darkTextColor, 5);
    
    drawText(scene, "CREATED BY SKIDGFX", 620, 900, darkTextColor, 4);
    
    drawText(scene, "X START  SQUARE QUIT", 680, 1000, darkTextColor, 3);
}

// Draw game over screen
void drawGameOver(Scene2D* scene) {
    scene->FrameBufferFill(bgColor);
    
    drawNumber(scene, 2048, 960, 100, darkTextColor, 8);
    
    drawText(scene, "FINAL SCORE", 720, 300, darkTextColor, 6);
    drawNumber(scene, score, 960, 380, darkTextColor, 8);
    
    drawText(scene, "HIGH SCORE", 740, 520, darkTextColor, 5);
    drawNumber(scene, highScore, 960, 590, darkTextColor, 5);
    
    if (hasWon) {
        drawText(scene, "YOU WIN", 810, 700, tile128Color, 7);
    } else {
        drawText(scene, "GAME OVER", 750, 700, tile32Color, 7);
    }
    
    drawText(scene, "TRIANGLE OR CIRCLE TO MENU", 520, 850, darkTextColor, 4);
    drawText(scene, "OPTIONS TO RESTART", 630, 920, darkTextColor, 4);
    drawText(scene, "X TO MENU", 800, 990, darkTextColor, 4);
}

// Draw the entire game
void drawGame(Scene2D* scene) {
    scene->FrameBufferFill(bgColor);
    
    drawNumber(scene, 2048, 960, 100, darkTextColor, 8);
    
    drawNumber(scene, score, 960, 180, darkTextColor, 5);
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            drawTile(scene, i, j, grid[i][j]);
        }
    }
    
    drawText(scene, "DPAD MOVE  OPTIONS RESTART  X MENU", 460, 950, darkTextColor, 3);
}

// Slide and merge tiles in one direction (left)
bool slideLeft() {
    bool moved = false;
    
    for (int i = 0; i < GRID_SIZE; i++) {
        int merged[GRID_SIZE] = {0};
        int writePos = 0;
        
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] != 0) {
                if (writePos > 0 && grid[i][writePos - 1] == grid[i][j] && !merged[writePos - 1]) {
                    grid[i][writePos - 1] *= 2;
                    score += grid[i][writePos - 1];
                    grid[i][j] = 0;
                    merged[writePos - 1] = 1;
                    moved = true;
                    
                    if (grid[i][writePos - 1] == WIN_TILE) {
                        hasWon = true;
                    }
                } else {
                    if (writePos != j) {
                        grid[i][writePos] = grid[i][j];
                        grid[i][j] = 0;
                        moved = true;
                    }
                    writePos++;
                }
            }
        }
    }
    
    return moved;
}

// Rotate grid 90 degrees clockwise
void rotateClockwise() {
    int temp[GRID_SIZE][GRID_SIZE];
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            temp[j][GRID_SIZE - 1 - i] = grid[i][j];
        }
    }
    
    memcpy(grid, temp, sizeof(grid));
}

// Move functions
bool moveLeft() { return slideLeft(); }
bool moveRight() { rotateClockwise(); rotateClockwise(); bool r = slideLeft(); rotateClockwise(); rotateClockwise(); return r; }
bool moveUp() { rotateClockwise(); rotateClockwise(); rotateClockwise(); bool r = slideLeft(); rotateClockwise(); return r; }
bool moveDown() { rotateClockwise(); bool r = slideLeft(); rotateClockwise(); rotateClockwise(); rotateClockwise(); return r; }

// Check if any moves are possible
bool canMove() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) return true;
        }
    }
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return true;
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return true;
        }
    }
    
    return false;
}

int main() {
    srand(time(NULL));
    
    setvbuf(stdout, NULL, _IONBF, 0);
    
    printf("Creating 2D scene\n");
    
    Scene2D* scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
    
    if(!scene->Init(0xC000000, 2)) {
        printf("Failed to initialize 2D scene\n");
        return 1;
    }
    
    printf("Initializing controller\n");
    
    Controller* controller = new Controller();
    
    if(!controller->Init(-1)) {
        printf("Failed to initialize controller\n");
        return 1;
    }
    
    // Initialize audio
    printf("Initializing audio\n");
    if(!initAudio()) {
        printf("Warning: Failed to initialize audio, continuing without sound\n");
    }
    
    printf("Starting game\n");
    
    bool running = true;
    bool lastUpPressed = false;
    bool lastDownPressed = false;
    bool lastLeftPressed = false;
    bool lastRightPressed = false;
    bool lastOptionsPressed = false;
    bool lastXPressed = false;
    bool lastTrianglePressed = false;
    bool lastCirclePressed = false;
    bool lastSquarePressed = false;
    
    // Main game loop
    while (running) {
        if (currentState == STATE_MENU) {
            drawMenu(scene);
            
            bool xPressed = controller->XPressed();
            if (xPressed && !lastXPressed) {
                currentState = STATE_PLAYING;
                initGrid();
                addRandomTile();
                addRandomTile();
            }
            lastXPressed = xPressed;
            
            bool squarePressed = controller->SquarePressed();
            if (squarePressed && !lastSquarePressed) {
                running = false;
            }
            lastSquarePressed = squarePressed;
            
        } else if (currentState == STATE_PLAYING) {
            drawGame(scene);
            
            bool moved = false;
            
            bool upPressed = controller->DpadUpPressed();
            if (upPressed && !lastUpPressed && !gameOver) {
                moved = moveUp();
            }
            lastUpPressed = upPressed;
            
            bool downPressed = controller->DpadDownPressed();
            if (downPressed && !lastDownPressed && !gameOver) {
                moved = moveDown();
            }
            lastDownPressed = downPressed;
            
            bool leftPressed = controller->DpadLeftPressed();
            if (leftPressed && !lastLeftPressed && !gameOver) {
                moved = moveLeft();
            }
            lastLeftPressed = leftPressed;
            
            bool rightPressed = controller->DpadRightPressed();
            if (rightPressed && !lastRightPressed && !gameOver) {
                moved = moveRight();
            }
            lastRightPressed = rightPressed;
            
            bool optionsPressed = controller->StartPressed();
            if (optionsPressed && !lastOptionsPressed) {
                initGrid();
                addRandomTile();
                addRandomTile();
                moved = false;
            }
            lastOptionsPressed = optionsPressed;
            
            bool xPressed = controller->XPressed();
            if (xPressed && !lastXPressed) {
                currentState = STATE_MENU;
                gameOver = false;
                hasWon = false;
            }
            lastXPressed = xPressed;
            
            if (moved) {
                addRandomTile();
                if (!canMove()) {
                    gameOver = true;
                    if (score > highScore) {
                        highScore = score;
                    }
                    currentState = STATE_GAME_OVER;
                }
            }
            
        } else if (currentState == STATE_GAME_OVER) {
            drawGameOver(scene);
            
            bool trianglePressed = controller->TrianglePressed();
            bool circlePressed = controller->CirclePressed();
            if ((trianglePressed && !lastTrianglePressed) || (circlePressed && !lastCirclePressed)) {
                currentState = STATE_MENU;
                gameOver = false;
                hasWon = false;
            }
            lastTrianglePressed = trianglePressed;
            lastCirclePressed = circlePressed;
            
            bool optionsPressed = controller->StartPressed();
            if (optionsPressed && !lastOptionsPressed) {
                currentState = STATE_PLAYING;
                initGrid();
                addRandomTile();
                addRandomTile();
                gameOver = false;
                hasWon = false;
            }
            lastOptionsPressed = optionsPressed;
            
            bool xPressed = controller->XPressed();
            if (xPressed && !lastXPressed) {
                currentState = STATE_MENU;
                gameOver = false;
                hasWon = false;
            }
            lastXPressed = xPressed;
        }
        
        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();
        frameID++;
    }
    
    printf("Game ended\n");
    
	// Cleanup and exit
    closeAudio();
    delete controller;
    delete scene;
    return 0;
}