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
    
private:
    void setButtonState(int state);
    
    int userID;
    int pad;
    OrbisPadData padData;
    int buttonState;
    int prevButtonState;
};

