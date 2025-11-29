#include "Renderer.h"
#include <string.h>
#include <stdio.h>

// Tile dimensions and positions
#define TILE_SIZE 150
#define TILE_PADDING 20
#define GRID_START_X 560    
#define GRID_START_Y 240

// Color definitions
static Color bgColor = { 0xFA, 0xF8, 0xEF, 0xFF };
static Color emptyTileColor = { 0xCD, 0xC1, 0xB4, 0xFF };
static Color tile2Color = { 0xEE, 0xE4, 0xDA, 0xFF };
static Color tile4Color = { 0xED, 0xE0, 0xC8, 0xFF };
static Color tile8Color = { 0xF2, 0xB1, 0x79, 0xFF };
static Color tile16Color = { 0xF5, 0x95, 0x63, 0xFF };
static Color tile32Color = { 0xF6, 0x7C, 0x5F, 0xFF };
static Color tile64Color = { 0xF6, 0x5E, 0x3B, 0xFF };
static Color tile128Color = { 0xED, 0xCF, 0x72, 0xFF };
static Color tile256Color = { 0xED, 0xCC, 0x61, 0xFF };
static Color tile512Color = { 0xED, 0xC8, 0x50, 0xFF };
static Color tile1024Color = { 0xED, 0xC5, 0x3F, 0xFF };
static Color tile2048Color = { 0xED, 0xC2, 0x2E, 0xFF };
static Color darkTextColor = { 0x77, 0x6E, 0x65, 0xFF };
static Color lightTextColor = { 0xF9, 0xF6, 0xF2, 0xFF };
static Color menuHighlightColor = { 0xF6, 0x7C, 0x5F, 0xFF };

// Simple 5x7 bitmap font for digits 0-9
static const uint8_t digitBitmaps[10][7] = {
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
static const uint8_t letterBitmaps[26][7] = {
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

void Renderer::Init() {
    // Nothing to initialize yet
}

void Renderer::drawChar(Scene2D* scene, char c, int x, int y, Color color, int scale) {
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

void Renderer::DrawText(Scene2D* scene, const char* text, int x, int y, Color color, int scale) {
    int xOffset = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        drawChar(scene, text[i], x + xOffset, y, color, scale);
        xOffset += (6 * scale);
    }
}

void Renderer::drawDigit(Scene2D* scene, int digit, int x, int y, Color color, int scale) {
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

void Renderer::DrawNumber(Scene2D* scene, int number, int x, int y, Color color, int scale) {
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

Color Renderer::getTileColor(int value) {
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

Color Renderer::getTextColor(int value) {
    if (value >= 8) {
        return lightTextColor;
    }
    return darkTextColor;
}

int Renderer::getNumberScale(int value) {
    if (value >= 1000) return 3;
    if (value >= 100) return 4;
    return 5;
}

void Renderer::DrawTile(Scene2D* scene, int row, int col, int value) {
    int x = GRID_START_X + col * (TILE_SIZE + TILE_PADDING);
    int y = GRID_START_Y + row * (TILE_SIZE + TILE_PADDING);
    
    Color tileColor = getTileColor(value);
    scene->DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, tileColor);
    
    if (value > 0) {
        Color textColor = getTextColor(value);
        int scale = getNumberScale(value);
        int centerX = x + TILE_SIZE / 2;
        int centerY = y + TILE_SIZE / 2 - (5 * scale) / 2;
        DrawNumber(scene, value, centerX, centerY, textColor, scale);
    }
}

void Renderer::DrawMenu(Scene2D* scene, int menuSelection, int highScore) {
    scene->FrameBufferFill(bgColor);
    
    DrawNumber(scene, 2048, 960, 200, darkTextColor, 16);
    
    Color startColor = (menuSelection == 0) ? menuHighlightColor : darkTextColor;
    DrawText(scene, "START GAME", 760, 450, startColor, 6);
    
    Color settingsColor = (menuSelection == 1) ? menuHighlightColor : darkTextColor;
    DrawText(scene, "SETTINGS", 820, 550, settingsColor, 6);
    
    DrawText(scene, "HIGH SCORE", 740, 700, darkTextColor, 5);
    DrawNumber(scene, highScore, 960, 770, darkTextColor, 5);
    
    DrawText(scene, "CREATED BY SKIDGFX", 744, 950, darkTextColor, 4);
    DrawText(scene, "X SELECT  UP DOWN NAVIGATE  SQUARE QUIT", 600, 1030, darkTextColor, 3);
}

void Renderer::DrawSettings(Scene2D* scene, int settingsSelection, int audioVolume) {
    scene->FrameBufferFill(bgColor);
    
    DrawText(scene, "SETTINGS", 820, 150, darkTextColor, 8);
    
    Color volumeColor = (settingsSelection == 0) ? menuHighlightColor : darkTextColor;
    DrawText(scene, "VOLUME", 700, 350, volumeColor, 6);
    
    // Volume bar
    int barX = 600;
    int barY = 450;
    int barWidth = 720;
    int barHeight = 40;
    
    scene->DrawRectangle(barX, barY, barWidth, barHeight, emptyTileColor);
    
    int fillWidth = (barWidth * audioVolume) / 100;
    if (fillWidth > 0) {
        scene->DrawRectangle(barX, barY, fillWidth, barHeight, menuHighlightColor);
    }
    
    char volBuf[16];
    snprintf(volBuf, sizeof(volBuf), "%d", audioVolume);
    DrawText(scene, volBuf, 1350, 455, darkTextColor, 5);
    DrawText(scene, "%", 1450, 455, darkTextColor, 5);
    
    Color backColor = (settingsSelection == 1) ? menuHighlightColor : darkTextColor;
    DrawText(scene, "BACK", 880, 650, backColor, 6);
    
    DrawText(scene, "X SELECT  UP DOWN NAVIGATE  LEFT RIGHT ADJUST", 546, 1030, darkTextColor, 3);
}

void Renderer::DrawGame(Scene2D* scene, int grid[4][4], int score) {
    scene->FrameBufferFill(bgColor);
    
    DrawNumber(scene, 2048, 960, 100, darkTextColor, 8);
    DrawNumber(scene, score, 960, 180, darkTextColor, 5);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            DrawTile(scene, i, j, grid[i][j]);
        }
    }
    
    DrawText(scene, "DPAD ANALOG SWIPE  OPTIONS RESTART  X MENU", 573, 950, darkTextColor, 3);
}

void Renderer::DrawGameOver(Scene2D* scene, int score, int highScore, bool hasWon) {
    scene->FrameBufferFill(bgColor);
    
    DrawNumber(scene, 2048, 960, 100, darkTextColor, 8);
    
    DrawText(scene, "FINAL SCORE", 720, 300, darkTextColor, 6);
    DrawNumber(scene, score, 960, 380, darkTextColor, 8);
    
    DrawText(scene, "HIGH SCORE", 740, 520, darkTextColor, 5);
    DrawNumber(scene, highScore, 960, 590, darkTextColor, 5);
    
    if (hasWon) {
        DrawText(scene, "YOU WIN", 810, 700, tile128Color, 7);
    } else {
        DrawText(scene, "GAME OVER", 750, 700, tile32Color, 7);
    }
    
    DrawText(scene, "TRIANGLE OR CIRCLE TO MENU", 636, 850, darkTextColor, 4);
    DrawText(scene, "OPTIONS TO RESTART", 744, 920, darkTextColor, 4);
    DrawText(scene, "X TO MENU", 852, 990, darkTextColor, 4);
}
