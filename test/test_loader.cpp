#include <Windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <cstdint>
#include <libloaderapi.h>
#include <minwindef.h>

using InitWithBrawlhallaDefaultsFunc = uint32_t (*)(const char*);
using DisposeFunc = void (*)();
using LoadBankFunc = uint32_t (*)(const char*);
using RegisterGameObjFunc = uint32_t (*)(uint32_t, const char*);
using SetDefaultListenerFunc = uint32_t (*)(uint32_t);
using SetPositionFunc = uint32_t (*)(uint32_t, float, float, float, float, float, float);
using SetRtpcValueFunc = uint32_t (*)(const char*, float, uint32_t);
using PostEventFunc = uint32_t (*)(const char*, uint32_t);
using TickFunc = uint32_t (*)();

int main() {
    std::cout << "====================================\n";
    std::cout << "WallyMusicJukebox Test - Music Player\n";
    std::cout << "====================================\n\n";

    const char* dllName = "WallyMusicJukebox.dll";
    
    std::cout << "Loading " << dllName << "...\n";
    
    // Load the DLL
    HMODULE hModule = LoadLibraryA(dllName);
    
    if (hModule == nullptr) {
        DWORD error = GetLastError();
        std::cerr << "Failed to load DLL! Error code: " << error << "\n";
        std::cerr << "Make sure " << dllName << " is in the same directory as this executable.\n";
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "DLL loaded successfully!\n\n";
    
    // Load function pointers
    auto pInitWithDefaults = (InitWithBrawlhallaDefaultsFunc)GetProcAddress(hModule, "InitWithBrawlhallaDefaults");
    auto pDispose = (DisposeFunc)GetProcAddress(hModule, "Dispose");
    auto pLoadBank = (LoadBankFunc)GetProcAddress(hModule, "LoadBank");
    auto pRegisterGameObj = (RegisterGameObjFunc)GetProcAddress(hModule, "RegisterGameObj");
    auto pSetDefaultListener = (SetDefaultListenerFunc)GetProcAddress(hModule, "SetDefaultListener");
    auto pSetPosition = (SetPositionFunc)GetProcAddress(hModule, "SetPosition");
    auto pSetRtpcValue = (SetRtpcValueFunc)GetProcAddress(hModule, "SetRtpcValue");
    auto pPostEvent = (PostEventFunc)GetProcAddress(hModule, "PostEvent");
    auto pTick = (TickFunc)GetProcAddress(hModule, "TickSoundEngineExtension");
    
    if ((pInitWithDefaults == nullptr) || (pDispose == nullptr) || (pLoadBank == nullptr) || (pRegisterGameObj == nullptr) || 
        (pSetDefaultListener == nullptr) || (pSetPosition == nullptr) || (pSetRtpcValue == nullptr) || (pPostEvent == nullptr) || (pTick == nullptr)) {
        std::cerr << "Failed to load required functions!\n";
        FreeLibrary(hModule);
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "Initializing Sound Engine with Brawlhalla defaults...\n";
    std::cout << "------------------------------------------------------\n";
    
    // Initialize with default Brawlhalla path (nullptr uses default)
    uint32_t result = pInitWithDefaults(nullptr);
    if (result != 1) {
        std::cerr << "\nInitialization failed!\n";
        FreeLibrary(hModule);
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "\n====================================\n";
    std::cout << "Setting up game objects...\n";
    std::cout << "====================================\n";
    
    const uint32_t SOUND_EMITTER = 1;
    const uint32_t MUSIC_EMITTER = 2;
    const uint32_t LISTENER = 0;
    
    pRegisterGameObj(SOUND_EMITTER, "Volume_Sound");
    pRegisterGameObj(MUSIC_EMITTER, "Volume_Music");
    pRegisterGameObj(LISTENER, "Listener");
    pSetDefaultListener(LISTENER);
    
    // Set listener position (0, 0, 0) facing forward
    pSetPosition(LISTENER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Listener Position set.\n";
    
    // Set emitter positions
    pSetPosition(SOUND_EMITTER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Emitter Position set.\n";
    pSetPosition(MUSIC_EMITTER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Emitter Position set.\n";
    
    // Set volume RTPCs
    constexpr float VOLUME_LEVEL = 6.0F;
    pSetRtpcValue("Volume_Sound", VOLUME_LEVEL, SOUND_EMITTER);
    std::cout << "RTPC value set.\n";
    pSetRtpcValue("Volume_Music", VOLUME_LEVEL, MUSIC_EMITTER);
    std::cout << "RTPC value set.\n";
    
    std::cout << "\n====================================\n";
    std::cout << "Loading music bank and playing...\n";
    std::cout << "====================================\n";
    
    // Load menu music bank
    result = pLoadBank("MUS_Menu.bnk");
    if (result != 1) {
        std::cerr << "Failed to load MUS_Menu.bnk - make sure Brawlhalla is installed!\n";
    } else {
        // Post the menu theme event
        uint32_t playingID = pPostEvent("MUS_Menu_Theme_Play", MUSIC_EMITTER);
        if (playingID != 0) {
            std::cout << "Event posted.\n";
            
            // Render audio
            pTick();
            
            std::cout << "\n====================================\n";
            std::cout << "Music should now be playing!\n";
            std::cout << "====================================\n";
        }
    }
    
    // Keep the program running
    std::cout << "\nPress Enter to stop and exit...";
    std::cin.get();
    
    // Cleanup
    std::cout << "\nShutting down Sound Engine...\n";
    pDispose();
    
    std::cout << "Unloading DLL...\n";
    FreeLibrary(hModule);
    std::cout << "Done. Exiting.\n";
    
    return 0;
}
