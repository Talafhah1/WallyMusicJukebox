#include "WwiseManager.hpp"
#include "exports.hpp"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <iostream>
#include <windows.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <stringapiset.h>
#include <winnls.h>

#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/AkTypes.h"
#include "AK/SoundEngine/Platforms/Windows/AkTypes.h"
#include "AK/SoundEngine/Platforms/Windows/AkWinSoundEngine.h"
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AkFilePackageLowLevelIOBlocking.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>
#include <AkFilePackage.h>
#include <AkFilePackageLUT.h>
#include <AK/Plugin/AkVorbisDecoderFactory.h>

// Wwise memory hooks
namespace AK
{
#ifdef WIN32
    void* AllocHook(size_t in_size) { return malloc(in_size); }
    void FreeHook(void* in_pMemAddress) { free(in_pMemAddress); }
    void* VirtualAllocHook(void* in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect) { 
        return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect); 
    }
    void VirtualFreeHook(void* in_pMemAddress, size_t in_size, DWORD in_dwFreeType) { 
        VirtualFree(in_pMemAddress, in_size, in_dwFreeType); 
    }
#endif
}

// Internal state
namespace {
    std::string g_basePath;
    bool g_initialized = false;
    CAkFilePackageLowLevelIOBlocking g_lowLevelIO;
}

// Helper to convert char* to wchar_t*
static std::wstring ToWideString(const char* str) {
    if (str == nullptr) { return L""; }
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr.data(), size);
    return wstr;
}

constexpr AkUInt32 MAX_MEMORY_POOLS = 20;
constexpr const wchar_t* DEFAULT_BRAWLHALLA_PATH = L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Brawlhalla\\audio\\pc";

extern "C" {

WALLYMUSICJUKEBOX_API uint32_t Init(const char* basePath) {
    if (g_initialized) {
        return AK_Fail;
    }
    
    // Initialize Memory Manager
    AkMemSettings memSettings;
    memSettings.uMaxNumPools = MAX_MEMORY_POOLS;
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
        std::cerr << "Failed to initialize Memory Manager.\n";
        return AK_Fail;
    }
    
    // Initialize Streaming Manager
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    if (AK::StreamMgr::Create(stmSettings) == nullptr) {
        std::cerr << "Failed to initialize Streaming Manager.\n";
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    
    // Create Streaming Device
    AkDeviceSettings devSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(devSettings);
    if (g_lowLevelIO.Init(devSettings) != AK_Success) {
        std::cerr << "Failed to create streaming device.\n";
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    
    // Initialize Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformSettings);
    if (AK::SoundEngine::Init(&initSettings, &platformSettings) != AK_Success) {
        std::cerr << "Failed to initialize Sound Engine.\n";
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    
    // Initialize Music Engine
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    if (AK::MusicEngine::Init(&musicInit) != AK_Success) {
        std::cerr << "Failed to initialize Music Engine.\n";
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    
    // Initialize Spatial Audio
    AkSpatialAudioInitSettings spaceSettings;
    if (AK::SpatialAudio::Init(spaceSettings) != AK_Success) {
        std::cerr << "Failed to initialize Spatial Audio.\n";
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    
    // Set base path if provided
    if (basePath != nullptr) {
        g_basePath = basePath;
        std::wstring wBasePath = ToWideString(basePath);
        g_lowLevelIO.SetBasePath(wBasePath.c_str());
    }
    
    // Set default language
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
    
    g_initialized = true;
    std::cout << "Sound Engine initialized successfully.\n";
    return AK_Success;
}

WALLYMUSICJUKEBOX_API uint32_t InitWithBrawlhallaDefaults(const char* basePath) {
    if (g_initialized) {
        return AK_Fail;
    }
    
    // Initialize Memory Manager
    AkMemSettings memSettings;
    memSettings.uMaxNumPools = MAX_MEMORY_POOLS;
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
        std::cerr << "Failed to initialize Memory Manager.\n";
        return AK_Fail;
    }
    std::cout << "Memory Manager initialised.\n";
    
    // Initialize Streaming Manager
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    if (AK::StreamMgr::Create(stmSettings) == nullptr) {
        std::cerr << "Failed to initialize Streaming Manager.\n";
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Streaming Manager initialised.\n";
    
    // Create Streaming Device
    AkDeviceSettings devSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(devSettings);
    if (g_lowLevelIO.Init(devSettings) != AK_Success) {
        std::cerr << "Failed to create streaming device.\n";
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Streaming Device created.\n";
    
    // Initialize Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformSettings);
    if (AK::SoundEngine::Init(&initSettings, &platformSettings) != AK_Success) {
        std::cerr << "Failed to initialize Sound Engine.\n";
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Sound Engine initialised.\n";
    
    // Initialize Music Engine
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    if (AK::MusicEngine::Init(&musicInit) != AK_Success) {
        std::cerr << "Failed to initialize Music Engine.\n";
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Music Engine initialised.\n";
    
    // Set base path (use default Brawlhalla path if not provided)
    const wchar_t* pathToUse = DEFAULT_BRAWLHALLA_PATH;
    if (basePath != nullptr) {
        g_basePath = basePath;
        std::wstring wBasePath = ToWideString(basePath);
        pathToUse = wBasePath.c_str();
        g_lowLevelIO.SetBasePath(pathToUse);
    } else {
        g_lowLevelIO.SetBasePath(pathToUse);
    }
    
    // Set default language
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
    
    // Load Init.bnk
    AkBankID initBankID;
    if (AK::SoundEngine::LoadBank(L"Init.bnk", AK_DEFAULT_POOL_ID, initBankID) != AK_Success) {
        std::cerr << "Failed to load Init.bnk\n";
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Sound Bank loaded: Init.bnk\n";
    
    // Load VOX_Announcer.bnk
    AkBankID announcerBankID;
    if (AK::SoundEngine::LoadBank(L"English(US)/VOX_Announcer.bnk", AK_DEFAULT_POOL_ID, announcerBankID) != AK_Success) {
        std::cerr << "Failed to load VOX_Announcer.bnk\n";
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Sound Bank loaded: VOX_Announcer.bnk\n";
    
    // Initialize Spatial Audio
    AkSpatialAudioInitSettings spaceSettings;
    if (AK::SpatialAudio::Init(spaceSettings) != AK_Success) {
        std::cerr << "Failed to initialize Spatial Audio.\n";
        AK::MusicEngine::Term();
        AK::SoundEngine::Term();
        g_lowLevelIO.Term();
        if (AK::IAkStreamMgr::Get() != nullptr) { AK::IAkStreamMgr::Get()->Destroy(); }
        AK::MemoryMgr::Term();
        return AK_Fail;
    }
    std::cout << "Spatial Audio initialised.\n";
    
    g_initialized = true;
    std::cout << "Sound Engine initialized with Brawlhalla defaults.\n";
    return AK_Success;
}

WALLYMUSICJUKEBOX_API void Dispose() {
    if (!g_initialized) {
        return;
    }
    
    AK::MusicEngine::Term();
    AK::SoundEngine::Term();
    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get() != nullptr) {
        AK::IAkStreamMgr::Get()->Destroy();
    }
    
    AK::MemoryMgr::Term();
    
    g_basePath.clear();
    g_initialized = false;
    std::cout << "Sound Engine terminated.\n";
}

WALLYMUSICJUKEBOX_API uint32_t SetBasePath(const char* path) {
    if (path == nullptr) {
        return AK_Fail;
    }
    
    g_basePath = path;
    std::wstring wPath = ToWideString(path);
    g_lowLevelIO.SetBasePath(wPath.c_str());
    
    return AK_Success;
}

WALLYMUSICJUKEBOX_API uint32_t LoadBank(const char* bank) {
    if (!g_initialized || (bank == nullptr)) {
        return AK_Fail;
    }
    
    std::wstring wBank = ToWideString(bank);
    AkBankID bankID;
    AKRESULT result = AK::SoundEngine::LoadBank(wBank.c_str(), AK_DEFAULT_POOL_ID, bankID);
    
    if (result == AK_Success) {
        std::cout << "Bank loaded: " << bank << "\n";
    } else {
        std::cerr << "Failed to load bank: " << bank << " (Error code: " << result << ")\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t UnloadBank(const char* bank) {
    if (!g_initialized || (bank == nullptr)) {
        return AK_Fail;
    }
    
    std::wstring wBank = ToWideString(bank);
    AKRESULT result = AK::SoundEngine::UnloadBank(wBank.c_str(), nullptr);
    
    if (result == AK_Success) {
        std::cout << "Bank unloaded: " << bank << "\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t RegisterGameObj(uint32_t gameObj, const char* name) {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    const char* objName = (name != nullptr) ? name : "";
    AKRESULT result = AK::SoundEngine::RegisterGameObj(gameObj, objName);
    
    if (result == AK_Success) {
        std::cout << "Game object registered: " << gameObj << " (" << objName << ")\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t UnregisterGameObj(uint32_t gameObj) {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    AKRESULT result = AK::SoundEngine::UnregisterGameObj(gameObj);
    
    if (result == AK_Success) {
        std::cout << "Game object unregistered: " << gameObj << "\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t SetDefaultListeners(const uint32_t* gameObjs, uint32_t length) {
    if (!g_initialized || (gameObjs == nullptr) || length == 0) {
        return AK_Fail;
    }
    
    AKRESULT result = AK::SoundEngine::SetDefaultListeners((AkGameObjectID*)gameObjs, length);
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t SetDefaultListener(uint32_t gameObj) {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    AkGameObjectID listener = gameObj;
    AKRESULT result = AK::SoundEngine::SetDefaultListeners(&listener, 1);
    
    if (result == AK_Success) {
        std::cout << "Default listener set: " << gameObj << "\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API void StopPlaying(uint32_t playingID, int32_t fadeDuration) {
    if (!g_initialized) {
        return;
    }
    
    AK::SoundEngine::StopPlayingID(playingID, fadeDuration);
}

WALLYMUSICJUKEBOX_API uint32_t PostEvent(const char* event, uint32_t gameObj) {
    if (!g_initialized || (event == nullptr)) {
        return 0;
    }
    
    std::wstring wEvent = ToWideString(event);
    AkPlayingID playingID = AK::SoundEngine::PostEvent(wEvent.c_str(), gameObj);
    
    if (playingID != AK_INVALID_PLAYING_ID) {
        std::cout << "Event posted: " << event << " on object " << gameObj << " (ID: " << playingID << ")\n";
    }
    
    return playingID;
}

WALLYMUSICJUKEBOX_API uint32_t SetRtpcValue(const char* gameParam, float value, uint32_t gameObj) {
    if (!g_initialized || (gameParam == nullptr)) {
        return AK_Fail;
    }
    
    std::wstring wParam = ToWideString(gameParam);
    AKRESULT result = AK::SoundEngine::SetRTPCValue(wParam.c_str(), value, gameObj);
    
    if (result == AK_Success) {
        std::cout << "RTPC set: " << gameParam << " = " << value << " on object " << gameObj << "\n";
    }
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t SetRtpcValueByPlayingID(const char* gameParam, float value, uint32_t playingID) {
    if (!g_initialized || (gameParam == nullptr)) {
        return AK_Fail;
    }
    
    std::wstring wParam = ToWideString(gameParam);
    AKRESULT result = AK::SoundEngine::SetRTPCValueByPlayingID(wParam.c_str(), value, playingID);
    
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t SetPosition(
    uint32_t gameObj,
    float positionX, float positionY, float positionZ,
    float orientationX, float orientationY, float orientationZ
) {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    AkSoundPosition soundPos;
    soundPos.SetPosition(AkVector(positionX, positionY, positionZ));
    soundPos.SetOrientation(AkVector(orientationX, orientationY, orientationZ), AkVector(0.F, 1.F, 0.F));
    
    AKRESULT result = AK::SoundEngine::SetPosition(gameObj, soundPos);
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t SetListenerPosition(
    float positionX, float positionY, float positionZ,
    float orientationFrontX, float orientationFrontY, float orientationFrontZ,
    float orientationTopX, float orientationTopY, float orientationTopZ
) {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    // Create listener position
    AkSoundPosition listenerPos;
    listenerPos.SetPosition(AkVector(positionX, positionY, positionZ));
    listenerPos.SetOrientation(
        AkVector(orientationFrontX, orientationFrontY, orientationFrontZ),
        AkVector(orientationTopX, orientationTopY, orientationTopZ)
    );
    
    // Set for default listener (ID 0)
    AKRESULT result = AK::SoundEngine::SetPosition(0, listenerPos);
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

WALLYMUSICJUKEBOX_API uint32_t TickSoundEngineExtension() {
    if (!g_initialized) {
        return AK_Fail;
    }
    
    AKRESULT result = AK::SoundEngine::RenderAudio();
    return (result == AK_Success) ? AK_Success : AK_Fail;
}

} // extern "C"


