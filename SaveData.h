#pragma once

#include <stdint.h>

// Save data structure
struct GameSaveData {
    uint32_t magic;        // Magic number to verify save file
    uint32_t version;      // Save version
    int highScore;         // High score
    int audioVolume;       // Audio volume (0-100)
    uint8_t padding[16];   // Reserved for future use
};

// Save/Load system
class SaveData {
public:
    static bool Save(int highScore, int audioVolume);
    static bool Load(int& highScore, int& audioVolume);
    
private:
    SaveData() = delete;
    
    static const uint32_t SAVE_MAGIC = 0x32303438; // "2048" in hex
    static const uint32_t SAVE_VERSION = 1;
};
