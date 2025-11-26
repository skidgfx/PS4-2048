#include <stdint.h>
#include <orbis/VideoOut.h>
#include <orbis/libkernel.h>

#ifndef GRAPHICS_H
#define GRAPHICS_H

// Color is used to pack together RGB information
struct Color { 
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;  // Alpha channel
};

class Scene2D
{
private:
    int video;
    void *videoMem;
    size_t videoMemSP;
    off_t directMemOff;
    size_t directMemAllocationSize;
    
    int width;
    int height;
    int depth;
    
    size_t frameBufferSize;
    int activeFrameBufferIdx;
    
    char **frameBuffers;
    OrbisVideoOutBufferAttribute attr;
    OrbisKernelEqueue flipQueue;
    
    bool initFlipQueue();
    bool allocateVideoMem(size_t size, int alignment);
    bool allocateFrameBuffers(int num);
    char *allocateDisplayMem(size_t size);
    void deallocateVideoMem();

public:
    Scene2D(int w, int h, int pixelDepth);
    
    bool Init(size_t memSize, int numFrameBuffers);
    
    void SetActiveFrameBuffer(int index);
    void SubmitFlip(int frameID);
    void FrameWait(int frameID);
    void FrameBufferSwap();
    void FrameBufferClear();
    void FrameBufferClear(Color color);
    void FrameBufferFill(Color color);
    
    void DrawPixel(int x, int y, Color color);
    void DrawRectangle(int x, int y, int w, int h, Color color);
};

#endif
