#include <stdio.h>
#include <sstream>
#include "App.h"

// Logging (required by graphics.cpp)
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
