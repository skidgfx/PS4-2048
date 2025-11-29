#include "App.h"
#include "Audio.h"
#include "SaveData.h"
#include "Renderer.h"
#include "Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cmath>

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

App::App() {
    scene = nullptr;
    controller = nullptr;
    running = false;
    frameID = 0;
    
    score = 0;
    highScore = 0;
    gameOver = false;
    hasWon = false;
    
    currentState = STATE_MENU;
    menuSelection = 0;
    settingsSelection = 0;
    
    analogInputCooldown = 0;
    
    lastTouchX = -1;
    lastTouchY = -1;
    lastTouchActive = false;
    
    lastUpPressed = false;
    lastDownPressed = false;
    lastLeftPressed = false;
    lastRightPressed = false;
    lastOptionsPressed = false;
    lastXPressed = false;
    lastTrianglePressed = false;
    lastCirclePressed = false;
    lastSquarePressed = false;
    
    memset(grid, 0, sizeof(grid));
}

App::~App() {
    Shutdown();
}

bool App::Init() {
    srand(time(NULL));
    
    printf("Creating 2D scene\n");
    scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
    
    if(!scene->Init(0xC000000, 2)) {
        printf("Failed to initialize 2D scene\n");
        return false;
    }
    
    printf("Initializing controller\n");
    controller = new Controller();
    
    if(!controller->Init(-1)) {
        printf("Failed to initialize controller\n");
        return false;
    }
    
    // Initialize input system
    Input::Init(controller);
    
    // Initialize renderer
    Renderer::Init();
    
    // Initialize audio
    printf("Initializing audio\n");
    if(!Audio::Init()) {
        printf("Warning: Failed to initialize audio, continuing without sound\n");
    }
    
    // Load save data
    printf("Loading save data\n");
    int loadedVolume = 100;
    if (SaveData::Load(highScore, loadedVolume)) {
        Audio::SetVolume(loadedVolume);
    }
    
    printf("Initialization complete\n");
    running = true;
    return true;
}

void App::Run() {
    printf("Starting game loop\n");
    
    while (running) {
        update();
        render();
        
        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();
        frameID++;
    }
}

void App::Shutdown() {
    printf("Shutting down\n");
    
    Audio::Shutdown();
    
    if (controller) {
        delete controller;
        controller = nullptr;
    }
    
    if (scene) {
        delete scene;
        scene = nullptr;
    }
}

void App::update() {
    // Decrease analog cooldown
    if (analogInputCooldown > 0) {
        analogInputCooldown--;
    }
    
    switch (currentState) {
        case STATE_MENU:
            handleMenuInput();
            break;
        case STATE_SETTINGS:
            handleSettingsInput();
            break;
        case STATE_PLAYING:
            handleGameInput();
            break;
        case STATE_GAME_OVER:
            handleGameOverInput();
            break;
    }
}

void App::render() {
    switch (currentState) {
        case STATE_MENU:
            Renderer::DrawMenu(scene, menuSelection, highScore);
            break;
        case STATE_SETTINGS:
            Renderer::DrawSettings(scene, settingsSelection, Audio::GetVolume());
            break;
        case STATE_PLAYING:
            Renderer::DrawGame(scene, grid, score);
            break;
        case STATE_GAME_OVER:
            Renderer::DrawGameOver(scene, score, highScore, hasWon);
            break;
    }
}

void App::handleMenuInput() {
    bool upPressed = controller->DpadUpPressed();
    if (upPressed && !lastUpPressed) {
        menuSelection = (menuSelection - 1 + 2) % 2;
    }
    lastUpPressed = upPressed;
    
    bool downPressed = controller->DpadDownPressed();
    if (downPressed && !lastDownPressed) {
        menuSelection = (menuSelection + 1) % 2;
    }
    lastDownPressed = downPressed;
    
    bool xPressed = controller->XPressed();
    if (xPressed && !lastXPressed) {
        if (menuSelection == 0) {
            currentState = STATE_PLAYING;
            initGrid();
            addRandomTile();
            addRandomTile();
        } else if (menuSelection == 1) {
            currentState = STATE_SETTINGS;
            settingsSelection = 0;
        }
    }
    lastXPressed = xPressed;
    
    bool squarePressed = controller->SquarePressed();
    if (squarePressed && !lastSquarePressed) {
        running = false;
    }
    lastSquarePressed = squarePressed;
}

void App::handleSettingsInput() {
    bool upPressed = controller->DpadUpPressed();
    if (upPressed && !lastUpPressed) {
        settingsSelection = (settingsSelection - 1 + 2) % 2;
    }
    lastUpPressed = upPressed;
    
    bool downPressed = controller->DpadDownPressed();
    if (downPressed && !lastDownPressed) {
        settingsSelection = (settingsSelection + 1) % 2;
    }
    lastDownPressed = downPressed;
    
    bool leftPressed = controller->DpadLeftPressed();
    if (leftPressed && !lastLeftPressed && settingsSelection == 0) {
        int volume = Audio::GetVolume();
        Audio::SetVolume(volume - 5);
    }
    lastLeftPressed = leftPressed;
    
    bool rightPressed = controller->DpadRightPressed();
    if (rightPressed && !lastRightPressed && settingsSelection == 0) {
        int volume = Audio::GetVolume();
        Audio::SetVolume(volume + 5);
    }
    lastRightPressed = rightPressed;
    
    bool xPressed = controller->XPressed();
    if (xPressed && !lastXPressed) {
        if (settingsSelection == 1) {
            currentState = STATE_MENU;
            menuSelection = 0;
            SaveData::Save(highScore, Audio::GetVolume());
        }
    }
    lastXPressed = xPressed;
    
    bool circlePressed = controller->CirclePressed();
    if (circlePressed && !lastCirclePressed) {
        currentState = STATE_MENU;
        menuSelection = 0;
        SaveData::Save(highScore, Audio::GetVolume());
    }
    lastCirclePressed = circlePressed;
}

void App::handleGameInput() {
    if (gameOver) return;
    
    bool moved = false;
    Direction dir = Input::GetDirectionInput(analogInputCooldown);
    
    switch (dir) {
        case DIR_UP:    moved = moveUp(); break;
        case DIR_DOWN:  moved = moveDown(); break;
        case DIR_LEFT:  moved = moveLeft(); break;
        case DIR_RIGHT: moved = moveRight(); break;
        default: break;
    }
    
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
                SaveData::Save(highScore, Audio::GetVolume());
            }
            currentState = STATE_GAME_OVER;
        }
    }
}

void App::handleGameOverInput() {
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

void App::initGrid() {
    memset(grid, 0, sizeof(grid));
    score = 0;
    gameOver = false;
    hasWon = false;
}

bool App::addRandomTile() {
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

bool App::slideLeft() {
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

void App::rotateClockwise() {
    int temp[GRID_SIZE][GRID_SIZE];
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            temp[j][GRID_SIZE - 1 - i] = grid[i][j];
        }
    }
    
    memcpy(grid, temp, sizeof(grid));
}

bool App::moveLeft() { 
    return slideLeft(); 
}

bool App::moveRight() { 
    rotateClockwise(); 
    rotateClockwise(); 
    bool r = slideLeft(); 
    rotateClockwise(); 
    rotateClockwise(); 
    return r; 
}

bool App::moveUp() { 
    rotateClockwise(); 
    rotateClockwise(); 
    rotateClockwise(); 
    bool r = slideLeft(); 
    rotateClockwise(); 
    return r; 
}

bool App::moveDown() { 
    rotateClockwise(); 
    bool r = slideLeft(); 
    rotateClockwise(); 
    rotateClockwise(); 
    rotateClockwise(); 
    return r; 
}

bool App::canMove() {
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
