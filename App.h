#pragma once

#include "graphics.h"
#include "controller.h"

// Game defines
#define GRID_SIZE 4
#define WIN_TILE 2048

// Game states
enum GameState {
    STATE_MENU,
    STATE_SETTINGS,
    STATE_PLAYING,
    STATE_GAME_OVER
};

class App {
public:
    App();
    ~App();
    
    bool Init();
    void Run();
    void Shutdown();
    
private:
    // Game state
    int grid[GRID_SIZE][GRID_SIZE];
    int score;
    int highScore;
    bool gameOver;
    bool hasWon;
    
    // UI state
    GameState currentState;
    int menuSelection;
    int settingsSelection;
    
    // Input state
    bool lastUpPressed;
    bool lastDownPressed;
    bool lastLeftPressed;
    bool lastRightPressed;
    bool lastOptionsPressed;
    bool lastXPressed;
    bool lastTrianglePressed;
    bool lastCirclePressed;
    bool lastSquarePressed;
    
    // Analog input timing
    int analogInputCooldown;
    
    // Touchpad state
    int lastTouchX;
    int lastTouchY;
    bool lastTouchActive;
    
    // Systems
    Scene2D* scene;
    Controller* controller;
    bool running;
    int frameID;
    
    // Game logic
    void initGrid();
    bool addRandomTile();
    bool slideLeft();
    void rotateClockwise();
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    bool canMove();
    
    // Input handling
    void handleMenuInput();
    void handleSettingsInput();
    void handleGameInput();
    void handleGameOverInput();
    
    // Update methods
    void update();
    void render();
};
