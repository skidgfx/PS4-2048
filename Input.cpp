#include "Input.h"
#include <math.h>

Controller* Input::controller = nullptr;

void Input::Init(Controller* ctrl) {
    controller = ctrl;
}

Direction Input::GetDirectionInput(int& analogCooldown) {
    if (!controller) return DIR_NONE;
    
    // Check D-Pad first (priority)
    if (controller->DpadUpPressed()) return DIR_UP;
    if (controller->DpadDownPressed()) return DIR_DOWN;
    if (controller->DpadLeftPressed()) return DIR_LEFT;
    if (controller->DpadRightPressed()) return DIR_RIGHT;
    
    // Check analog stick (with cooldown)
    if (analogCooldown == 0) {
        float stickX = controller->GetLeftStickX();
        float stickY = controller->GetLeftStickY();
        
        if (fabsf(stickY) > ANALOG_DEADZONE && fabsf(stickY) > fabsf(stickX)) {
            analogCooldown = ANALOG_INPUT_DELAY;
            if (stickY < 0) return DIR_UP;
            else return DIR_DOWN;
        } else if (fabsf(stickX) > ANALOG_DEADZONE) {
            analogCooldown = ANALOG_INPUT_DELAY;
            if (stickX < 0) return DIR_LEFT;
            else return DIR_RIGHT;
        }
    }
    
    // Check touchpad swipe (disabled for now - SDK limitations)
    // if (controller->IsTouchpadTouched()) { ... }
    
    return DIR_NONE;
}

bool Input::IsConfirmPressed() {
    return controller ? controller->XPressed() : false;
}

bool Input::IsCancelPressed() {
    return controller ? controller->CirclePressed() : false;
}

bool Input::IsQuitPressed() {
    return controller ? controller->SquarePressed() : false;
}

bool Input::IsRestartPressed() {
    return controller ? controller->StartPressed() : false;
}
