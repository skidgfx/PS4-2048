#pragma once

#include <orbis/Pad.h>
#include <orbis/UserService.h>

class Controller
{
public:
    Controller();
    ~Controller();
    
    bool Init(int controllerUserID);
    
    bool CheckButtonsPressed(int stateToCheck);
    
    bool TrianglePressed();
    bool CirclePressed();
    bool XPressed();
    bool SquarePressed();
    bool L1Pressed();
    bool L2Pressed();
    bool R1Pressed();
    bool R2Pressed();
    bool L3Pressed();
    bool R3Pressed();
    bool StartPressed();
    bool DpadUpPressed();
    bool DpadRightPressed();
    bool DpadDownPressed();
    bool DpadLeftPressed();
    bool TouchpadPressed();
    
    // Analog stick methods
    float GetLeftStickX();
    float GetLeftStickY();
    float GetRightStickX();
    float GetRightStickY();
    
    // Touchpad methods
    int GetTouchpadX();
    int GetTouchpadY();
    bool IsTouchpadTouched();
    
private:
    void setButtonState(int state);
    float normalizeStickValue(uint8_t value);
    
    int userID;
    int pad;
    OrbisPadData padData;
    int buttonState;
    int prevButtonState;
};

