#pragma once

#include "controller.h"

// Input handling constants
#define ANALOG_INPUT_DELAY 15 // Frames to wait between analog inputs (~1 second at 15 FPS)
#define ANALOG_DEADZONE 0.5f

// Direction enum
enum Direction {
    DIR_NONE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

// Input handler
class Input {
public:
    static void Init(Controller* ctrl);
    static Direction GetDirectionInput(int& analogCooldown);
    static bool IsConfirmPressed();
    static bool IsCancelPressed();
    static bool IsQuitPressed();
    static bool IsRestartPressed();
    
private:
    Input() = delete;
    static Controller* controller;
};
