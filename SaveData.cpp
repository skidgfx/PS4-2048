#include "SaveData.h"
#include <stdio.h>
#include <string.h>

bool SaveData::Save(int highScore, int audioVolume) {
    // Try multiple writable locations
    const char* savePaths[] = {
        "/user/home/2048_save.dat",
        "/mnt/usb0/2048_save.dat",
        "/data/2048_save.dat"
    };
    
    // Create save data structure
    GameSaveData saveData;
    saveData.magic = SAVE_MAGIC;
    saveData.version = SAVE_VERSION;
    saveData.highScore = highScore;
    saveData.audioVolume = audioVolume;
    memset(saveData.padding, 0, sizeof(saveData.padding));
    
    // Try each path until one works
    for (int i = 0; i < 3; i++) {
        FILE* file = fopen(savePaths[i], "wb");
        if (file) {
            size_t written = fwrite(&saveData, sizeof(GameSaveData), 1, file);
            fclose(file);
            
            if (written == 1) {
                printf("[INFO] Game data saved successfully to %s\n", savePaths[i]);
                return true;
            }
        }
    }
    
    printf("[ERROR] Failed to save game data to any location\n");
    return false;
}

bool SaveData::Load(int& highScore, int& audioVolume) {
    // Try multiple possible save locations
    const char* savePaths[] = {
        "/user/home/2048_save.dat",
        "/mnt/usb0/2048_save.dat",
        "/data/2048_save.dat"
    };
    
    for (int i = 0; i < 3; i++) {
        FILE* file = fopen(savePaths[i], "rb");
        if (file) {
            GameSaveData saveData;
            size_t read = fread(&saveData, sizeof(GameSaveData), 1, file);
            fclose(file);
            
            if (read == 1 && saveData.magic == SAVE_MAGIC) {
                // Load data
                highScore = saveData.highScore;
                audioVolume = saveData.audioVolume;
                
                printf("[INFO] Game data loaded successfully from %s\n", savePaths[i]);
                printf("[INFO] High Score: %d, Volume: %d%%\n", highScore, audioVolume);
                return true;
            }
        }
    }
    
    printf("[INFO] No save data found (first run)\n");
    return false;
}
