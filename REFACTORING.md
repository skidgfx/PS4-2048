# Code Refactoring Plan for PS4-2048

## Summary
This document outlines the refactored code structure for better organization and maintainability.

## New File Structure

```
PS4-2048/
??? Audio.cpp/h          ? CREATED - Audio system handling
??? SaveData.cpp/h       ? CREATED - Save/load functionality  
??? Renderer.h           ? CREATED - Drawing functions header
??? Input.h              ? CREATED - Input handling header
??? App.h                ? CREATED - Main game logic header
??? main.cpp             ? NEEDS UPDATE - Entry point only
??? controller.cpp/h     ? KEEP - Controller wrapper
??? graphics.cpp/h       ? KEEP - Graphics wrapper
??? dr_wav.h             ? KEEP - Audio decoder
??? build.bat            ? NEEDS UPDATE - Add new files
??? .gitignore           ? NEEDS UPDATE - Add pkg.gp4, *.pkg, etc.
```

## Files Created

### 1. Audio.h / Audio.cpp
- Static class for audio management
- Methods: `Init()`, `Shutdown()`, `SetVolume()`, `GetVolume()`
- Encapsulates all audio threading and playback

### 2. SaveData.h / SaveData.cpp  
- Static class for save/load operations
- Methods: `Save(highScore, volume)`, `Load(highScore, volume)`
- Handles multiple save locations with fallback

### 3. Renderer.h (header only for now)
- Static class for all drawing operations
- Will contain: `DrawMenu()`, `DrawSettings()`, `DrawGame()`, `DrawGameOver()`
- Also has primitive functions: `DrawText()`, `DrawNumber()`, `DrawTile()`

### 4. Input.h (header only for now)
- Static class for input handling
- Will contain: `GetDirectionInput()`, `IsConfirmPressed()`, etc.
- Handles D-Pad, Analog, and Touchpad

### 5. App.h (header only for now)
- Main game class
- Contains all game logic and state
- Methods: `Init()`, `Run()`, `Shutdown()`

## Next Steps

### Step 1: Complete Implementation Files
Need to create:
- `Renderer.cpp` - Move all drawing code from main.cpp
- `Input.cpp` - Move input handling from main.cpp  
- `App.cpp` - Move game logic from main.cpp

### Step 2: Update main.cpp
```cpp
#include <stdio.h>
#include <sstream>
#include "App.h"

std::stringstream debugLogStream;

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Starting 2048 for PS4\n");
    
    App app;
    
    if (!app.Init()) {
        printf("Failed to initialize game\n");
        return 1;
    }
    
    app.Run();
    app.Shutdown();
    
    printf("Game ended\n");
    return 0;
}
```

### Step 3: Update build.bat
Add new source files to compilation:
```batch
for %%f in (*.cpp) do (
    clang++ --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables \
            -I"%OO_PS4_TOOLCHAIN%\\include" \
            -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" \
            %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.cpp
)
```

This will automatically compile:
- App.cpp
- Audio.cpp
- SaveData.cpp
- Renderer.cpp (when created)
- Input.cpp (when created)
- controller.cpp
- graphics.cpp

### Step 4: Update .gitignore
Add these lines at the top:
```gitignore
# PS4 Build outputs
*.o
*.elf
*.oelf
*.bin
eboot.bin
pkg.gp4
*.pkg

# param.sfo (generated)
sce_sys/param.sfo
```

## Benefits of This Structure

### Code Organization
- ? Separated concerns (Audio, Save, Render, Input, Game Logic)
- ? Easy to find and modify specific functionality
- ? Reduced file size (main.cpp was 900+ lines!)

### Maintainability
- ? Changes to audio don't affect game logic
- ? Can swap out save system easily
- ? Renderer can be unit tested independently

### Scalability
- ? Easy to add new features (e.g., multiple save slots)
- ? Can add sound effects to Audio class
- ? Can extend Renderer for animations

### Build System
- ? Parallel compilation of separate modules
- ? Only recompile changed modules
- ? Cleaner git history per module

## Implementation Order

1. ? Create headers (DONE)
2. ? Implement Audio.cpp (DONE)
3. ? Implement SaveData.cpp (DONE)
4. ? Create Renderer.cpp  
5. ? Create Input.cpp
6. ? Create App.cpp
7. ? Update main.cpp
8. ? Update build.bat
9. ? Update .gitignore
10. ? Test build

Would you like me to continue with steps 4-6 (creating the remaining .cpp files)?
