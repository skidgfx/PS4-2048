#include "Audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <orbis/AudioOut.h>
#include <orbis/Sysmodule.h>
#include <orbis/UserService.h>
#include <orbis/libkernel.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 256

// Static audio state
static int32_t audioHandle = -1;
static bool audioInitialized = false;
static bool audioThreadRunning = false;
static pthread_t audioThread;
static drwav_int16* audioData = nullptr;
static size_t audioSampleCount = 0;
static size_t audioCurrentSample = 0;
static int audioVolume = 100; // 0-100%

// Audio playback thread
void* Audio::audioPlaybackThread(void* arg) {
    drwav_int16* pSample = nullptr;
    drwav_int16 adjustedBuffer[AUDIO_BUFFER_SIZE * AUDIO_CHANNELS];
    
    while (audioThreadRunning) {
        if (!audioData || audioSampleCount == 0) {
            sceKernelUsleep(10000); // 10ms
            continue;
        }
        
        pSample = &audioData[audioCurrentSample];
        
        // Apply volume adjustment
        for (int i = 0; i < AUDIO_BUFFER_SIZE * AUDIO_CHANNELS; i++) {
            adjustedBuffer[i] = (drwav_int16)((pSample[i] * audioVolume) / 100);
        }
        
        // Wait for completion
        sceAudioOutOutput(audioHandle, NULL);
        
        // Output audio
        if (sceAudioOutOutput(audioHandle, adjustedBuffer) < 0) {
            printf("Failed to output audio\n");
            sceKernelUsleep(10000);
            continue;
        }
        
        // Move to next chunk (256 samples * 2 channels)
        audioCurrentSample += AUDIO_BUFFER_SIZE * 2;
        
        // Loop back to beginning
        if (audioCurrentSample >= audioSampleCount) {
            audioCurrentSample = 0;
        }
    }
    
    return nullptr;
}

bool Audio::Init() {
    int rc;
    OrbisUserServiceUserId userId = ORBIS_USER_SERVICE_USER_ID_SYSTEM;
    
    sceUserServiceInitialize(NULL);
    
    // Initialize audio output library
    rc = sceAudioOutInit();
    if (rc != 0) {
        printf("[ERROR] Failed to initialize audio output\n");
        return false;
    }
    
    // Open a handle to audio output device
    audioHandle = sceAudioOutOpen(userId, ORBIS_AUDIO_OUT_PORT_TYPE_MAIN, 0, 
                                   AUDIO_BUFFER_SIZE, AUDIO_SAMPLE_RATE, 
                                   ORBIS_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
    
    if (audioHandle <= 0) {
        printf("[ERROR] Failed to open audio on main port\n");
        return false;
    }
    
    // Decode a wav file to play
    drwav wav;
    if (!drwav_init_file(&wav, "/app0/assets/audio/bg.wav", NULL)) {
        printf("[ERROR] Failed to decode wav file\n");
        return false;
    }
    
    printf("[INFO] WAV loaded: %d Hz, %d channels\n", wav.sampleRate, wav.channels);
    
    // Calculate the sample count and allocate a buffer
    audioSampleCount = wav.totalPCMFrameCount * wav.channels;
    audioData = (drwav_int16*)malloc(audioSampleCount * sizeof(drwav_int16));
    
    if (!audioData) {
        printf("[ERROR] Failed to allocate audio memory\n");
        drwav_uninit(&wav);
        return false;
    }
    
    // Decode the wav into audioData
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData);
    drwav_uninit(&wav);
    
    printf("[INFO] Loaded %zu samples of audio data\n", audioSampleCount);
    
    // Start audio playback thread
    audioThreadRunning = true;
    if (pthread_create(&audioThread, nullptr, audioPlaybackThread, nullptr) != 0) {
        printf("[ERROR] Failed to create audio thread\n");
        free(audioData);
        return false;
    }
    
    printf("[INFO] Audio initialized successfully\n");
    audioInitialized = true;
    return true;
}

void Audio::Shutdown() {
    if (audioThreadRunning) {
        audioThreadRunning = false;
        pthread_join(audioThread, nullptr);
    }
    
    if (audioData) {
        free(audioData);
        audioData = nullptr;
    }
    
    if (audioHandle > 0) {
        sceAudioOutClose(audioHandle);
        audioHandle = -1;
    }
    
    audioInitialized = false;
}

void Audio::SetVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    audioVolume = volume;
}

int Audio::GetVolume() {
    return audioVolume;
}

bool Audio::IsInitialized() {
    return audioInitialized;
}
