#pragma once

#include "graphics.h"

// Renderer class for all drawing operations
class Renderer {
public:
    static void Init();
    
    // Screen drawing
    static void DrawMenu(Scene2D* scene, int menuSelection, int highScore);
    static void DrawSettings(Scene2D* scene, int settingsSelection, int audioVolume);
    static void DrawGame(Scene2D* scene, int grid[4][4], int score);
    static void DrawGameOver(Scene2D* scene, int score, int highScore, bool hasWon);
    
    // Primitive drawing
    static void DrawText(Scene2D* scene, const char* text, int x, int y, Color color, int scale);
    static void DrawNumber(Scene2D* scene, int number, int x, int y, Color color, int scale);
    static void DrawTile(Scene2D* scene, int row, int col, int value);
    
private:
    Renderer() = delete;
    
    static void drawChar(Scene2D* scene, char c, int x, int y, Color color, int scale);
    static void drawDigit(Scene2D* scene, int digit, int x, int y, Color color, int scale);
    static Color getTileColor(int value);
    static Color getTextColor(int value);
    static int getNumberScale(int value);
};
