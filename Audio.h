#pragma once

#include <stdint.h>

// Audio system interface
class Audio {
public:
    static bool Init();
    static void Shutdown();
    static void SetVolume(int volume); // 0-100
    static int GetVolume();
    static bool IsInitialized();
    
private:
    Audio() = delete;
    static void* audioPlaybackThread(void* arg);
};
