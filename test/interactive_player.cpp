#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdint>

#include "WwiseManager.hpp"

constexpr float DEFAULT_VOLUME = 6.0F;
constexpr uint32_t FADE_DURATION_MS = 500;
constexpr uint32_t FADE_OFFSET_MS = 100;
constexpr uint32_t SLEEP_DURATION_MS = 10;
constexpr uint32_t SOUND_EMITTER = 1;
constexpr uint32_t MUSIC_EMITTER = 2;
constexpr uint32_t LISTENER = 0;
constexpr const char* DEFAULT_BRAWLHALLA_PATH = R"(C:\Program Files (x86)\Steam\steamapps\common\Brawlhalla\audio\pc)";

// Global state for playback control
std::atomic<bool> g_running(true);
std::atomic<uint32_t> g_currentPlayingID(0);
std::string g_audioBasePath;
std::string g_currentLoadedBank;

// Audio rendering thread
void audioThread() {
    while (g_running) {
        TickSoundEngineExtension();
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_DURATION_MS));
    }
}

bool initializeEngine(const char* audioPath) {
    std::cout << "Initializing Sound Engine...\n";
    std::cout << "------------------------------------------------------\n";
    
    // Store the audio path for later use
    if (audioPath != nullptr) {
        g_audioBasePath = audioPath;
    } else {
        g_audioBasePath = DEFAULT_BRAWLHALLA_PATH;
    }
    
    uint32_t result = InitWithBrawlhallaDefaults(audioPath);
    if (result != 1) {
        std::cerr << "\nInitialization failed!\n";
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return false;
    }
    return true;
}

void setupGameObjects() {
    std::cout << "\n====================================\n";
    std::cout << "Setting up game objects...\n";
    std::cout << "====================================\n";

    RegisterGameObj(SOUND_EMITTER, "Volume_Sound");
    RegisterGameObj(MUSIC_EMITTER, "Volume_Music");
    RegisterGameObj(LISTENER, "Listener");
    SetDefaultListener(LISTENER);

    // Set positions
    SetPosition(LISTENER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Listener Position set.\n";
    SetPosition(SOUND_EMITTER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Sound Emitter Position set.\n";
    SetPosition(MUSIC_EMITTER, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F);
    std::cout << "Music Emitter Position set.\n";

    // Set volume RTPCs
    SetRtpcValue("Volume_Sound", DEFAULT_VOLUME, SOUND_EMITTER);
    std::cout << "Sound RTPC value set.\n";
    SetRtpcValue("Volume_Music", DEFAULT_VOLUME, MUSIC_EMITTER);
    std::cout << "Music RTPC value set.\n";
}

bool loadBank(const std::string& bankName) {
    // Skip Init.bnk - it's already loaded by InitWithBrawlhallaDefaults
    if (bankName == "Init.bnk" || bankName == "Init") {
        std::cout << "Skipping Init.bnk (already loaded)\n";
        return true;
    }
    
    // Add .bnk extension if not present
    std::string bankFile = bankName;
    if (bankFile.find(".bnk") == std::string::npos) {
        bankFile += ".bnk";
    }
    
    std::cout << "Loading bank: " << bankFile << "...\n";
    if (LoadBank(bankFile.c_str()) == 1) {
        std::cout << "Bank loaded successfully!\n";
        return true;
    }
    
    std::cerr << "Failed to load bank: " << bankFile << "\n";
    return false;
}

void unloadCurrentBank() {
    if (!g_currentLoadedBank.empty()) {
        std::cout << "Unloading bank: " << g_currentLoadedBank << "...\n";
        UnloadBank(g_currentLoadedBank.c_str());
        g_currentLoadedBank.clear();
    }
}

void printInstructions() {
    std::cout << "\n====================================\n";
    std::cout << "Interactive Event Player\n";
    std::cout << "====================================\n";
    std::cout << "Commands:\n";
    std::cout << "  - Format: BankName::EventName\n";
    std::cout << "    Example: MUS_Menu::MUS_Menu_Theme_Play\n";
    std::cout << "  - 'stop' - Stop current playback and unload bank\n";
    std::cout << "  - 'music' - Use music emitter for next event\n";
    std::cout << "  - 'sound' - Use sound emitter for next event\n";
    std::cout << "  - 'quit' or 'exit' - Exit the program\n";
    std::cout << "====================================\n\n";
}

std::string trimWhitespace(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool handleStopCommand() {
    uint32_t playingID = g_currentPlayingID.load();
    if (playingID != 0) {
        std::cout << "Stopping playback (ID: " << playingID << ")...\n";
        StopPlaying(playingID, FADE_DURATION_MS);
        g_currentPlayingID.store(0);
    } else {
        std::cout << "Nothing is playing.\n";
    }
    
    // Unload the current bank
    unloadCurrentBank();
    return true;
}

bool handleEmitterSwitch(const std::string& input, uint32_t& currentEmitter, const char*& emitterName) {
    if (input == "music") {
        currentEmitter = MUSIC_EMITTER;
        emitterName = "Music";
        std::cout << "Switched to Music emitter.\n";
        return true;
    }
    if (input == "sound") {
        currentEmitter = SOUND_EMITTER;
        emitterName = "Sound";
        std::cout << "Switched to Sound emitter.\n";
        return true;
    }
    return false;
}

void handleEventPost(const std::string& input, uint32_t emitter, const char* emitterName) {
    // Parse Bank::Event format
    size_t separator = input.find("::");
    if (separator == std::string::npos) {
        std::cerr << "Invalid format! Use: BankName::EventName\n";
        std::cerr << "Example: MUS_Menu::MUS_Menu_Theme_Play\n\n";
        return;
    }
    
    std::string bankName = input.substr(0, separator);
    std::string eventName = input.substr(separator + 2);
    
    if (bankName.empty() || eventName.empty()) {
        std::cerr << "Invalid format! Both bank and event names are required.\n\n";
        return;
    }
    
    // Stop current playback if any
    uint32_t playingID = g_currentPlayingID.load();
    if (playingID != 0) {
        std::cout << "Stopping current playback...\n";
        StopPlaying(playingID, FADE_DURATION_MS);
        g_currentPlayingID.store(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(FADE_DURATION_MS + FADE_OFFSET_MS));
    }
    
    // Unload previous bank if different
    if (!g_currentLoadedBank.empty() && g_currentLoadedBank != bankName) {
        unloadCurrentBank();
    }
    
    // Load the new bank if not already loaded
    if (g_currentLoadedBank != bankName) {
        if (!loadBank(bankName)) {
            std::cerr << "Cannot post event - bank failed to load.\n\n";
            return;
        }
        g_currentLoadedBank = bankName;
    }
    
    // Post the event
    std::cout << "Posting event: " << eventName << " on " << emitterName << " emitter...\n";
    playingID = PostEvent(eventName.c_str(), emitter);
    
    if (playingID != 0) {
        g_currentPlayingID.store(playingID);
        std::cout << "Event posted successfully! (Playing ID: " << playingID << ")\n";
    } else {
        std::cout << "Failed to post event (ID: 0). Event may not exist in bank.\n";
    }
    
    std::cout << '\n' << std::flush;
}

void runInteractiveLoop() {
    std::string input;
    uint32_t currentEmitter = MUSIC_EMITTER;
    const char* emitterName = "Music";

    while (true) {
        std::cout << "[" << emitterName << " Emitter] Bank::Event: ";
        std::getline(std::cin, input);

        input = trimWhitespace(input);

        if (input.empty()) {
            continue;
        }

        if (input == "quit" || input == "exit") {
            std::cout << "\nExiting...\n";
            break;
        }
        
        if (input == "stop") {
            handleStopCommand();
            continue;
        }
        
        if (handleEmitterSwitch(input, currentEmitter, emitterName)) {
            continue;
        }

        handleEventPost(input, currentEmitter, emitterName);
    }
}

int main(int argc, char* argv[]) {
    std::cout << "====================================\n";
    std::cout << "WallyMusicJukebox Interactive Player\n";
    std::cout << "====================================\n\n";

    // Determine Brawlhalla audio path
    const char* audioPath = nullptr;
    if (argc > 1) {
        audioPath = argv[1];
        std::cout << "Using custom audio path: " << audioPath << "\n\n";
    } else {
        std::cout << "Using default Brawlhalla audio path\n\n";
    }

    if (!initializeEngine(audioPath)) {
        return 1;
    }

    setupGameObjects();

    // Start audio rendering thread
    std::cout << "\nStarting audio engine...\n";
    std::thread audioRenderThread(audioThread);

    printInstructions();
    runInteractiveLoop();

    // Stop audio thread
    std::cout << "\nStopping audio engine...\n";
    g_running = false;
    audioRenderThread.join();

    // Cleanup
    std::cout << "Shutting down Sound Engine...\n";
    Dispose();

    std::cout << "Done. Goodbye!\n";
    return 0;
}
