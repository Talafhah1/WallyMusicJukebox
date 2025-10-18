#pragma once

#include "exports.hpp"
#include <cstdint>

// C-style API matching Brawlhalla's SoundEngineExtension interface
extern "C" {
    // Initialize the sound engine with base path for sound banks
    WALLYMUSICJUKEBOX_API uint32_t Init(const char* basePath);
    
    // Initialize with Brawlhalla's default settings (loads Init.bnk and VOX_Announcer.bnk)
    // Pass nullptr to use default Brawlhalla audio path
    WALLYMUSICJUKEBOX_API uint32_t InitWithBrawlhallaDefaults(const char* basePath);
    
    // Cleanup and shutdown the sound engine
    WALLYMUSICJUKEBOX_API void Dispose();
    
    // Set the base path for sound banks
    WALLYMUSICJUKEBOX_API uint32_t SetBasePath(const char* path);
    
    // Load a sound bank by name
    WALLYMUSICJUKEBOX_API uint32_t LoadBank(const char* bank);
    
    // Unload a sound bank by name
    WALLYMUSICJUKEBOX_API uint32_t UnloadBank(const char* bank);
    
    // Register a game object with optional name
    WALLYMUSICJUKEBOX_API uint32_t RegisterGameObj(uint32_t gameObj, const char* name);
    
    // Unregister a game object
    WALLYMUSICJUKEBOX_API uint32_t UnregisterGameObj(uint32_t gameObj);
    
    // Set multiple default listeners
    WALLYMUSICJUKEBOX_API uint32_t SetDefaultListeners(const uint32_t* gameObjs, uint32_t length);
    
    // Set a single default listener
    WALLYMUSICJUKEBOX_API uint32_t SetDefaultListener(uint32_t gameObj);
    
    // Stop a playing sound by ID with optional fade duration (ms)
    WALLYMUSICJUKEBOX_API void StopPlaying(uint32_t playingID, int32_t fadeDuration);
    
    // Post an event to a game object (0 = default)
    WALLYMUSICJUKEBOX_API uint32_t PostEvent(const char* event, uint32_t gameObj);
    
    // Set RTPC (Real-Time Parameter Control) value for a game object
    WALLYMUSICJUKEBOX_API uint32_t SetRtpcValue(const char* gameParam, float value, uint32_t gameObj);
    
    // Set RTPC value by playing ID
    WALLYMUSICJUKEBOX_API uint32_t SetRtpcValueByPlayingID(const char* gameParam, float value, uint32_t playingID);
    
    // Set 3D position and orientation for a game object
    WALLYMUSICJUKEBOX_API uint32_t SetPosition(
        uint32_t gameObj,
        float positionX, float positionY, float positionZ,
        float orientationX, float orientationY, float orientationZ
    );
    
    // Set 3D listener position and orientation
    WALLYMUSICJUKEBOX_API uint32_t SetListenerPosition(
        float positionX, float positionY, float positionZ,
        float orientationFrontX, float orientationFrontY, float orientationFrontZ,
        float orientationTopX, float orientationTopY, float orientationTopZ
    );
    
    // Tick/Update the sound engine (call every frame)
    WALLYMUSICJUKEBOX_API uint32_t TickSoundEngineExtension();
}

