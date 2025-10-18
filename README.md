# WallyMusicJukebox

An open-source recreation of Brawlhalla's Wwise-based SoundEngineExtension ANE. 

## Prerequisites

- CMake 3.20 or higher
- One of the following compilers:
  - Visual Studio 2017/2019/2022 (MSVC) - uses vc150 libraries
  - MinGW-w64 or MSYS2 (GCC) - compatible with vc150 libraries
  - LLVM/Clang with Ninja build system - compatible with vc150 libraries
- Audiokinetic Wwise SDK **2017.2.10.6745** (exact version required for Brawlhalla compatibility)
  - Must have **vc150** libraries (x64_vc150 or Win32_vc150)

## Project Features

- **C++20** standard
- **Multi-compiler support**: MSVC, GCC, Clang
- **Environment-based configuration** via `.env` file
- **CMake build system** with automatic Wwise SDK detection
- **Cross-platform build scripts**: Batch, PowerShell, and Bash
- **clangd integration** for excellent IntelliSense in VS Code

## Configuration

### Setting Wwise SDK Path

**Important:** This project requires Audiokinetic Wwise SDK version **2017.2.10.6745** exactly with **vc150** libraries. This is the only version compatible with Brawlhalla.

The easiest way to configure is by editing the `.env` file:

1. Copy `.env.example` to `.env`:
   ```cmd
   copy .env.example .env
   ```

2. Edit `.env` and set your Wwise SDK installation path:
   ```env
   WWISE_SDK_PATH=C:\Program Files (x86)\Audiokinetic\Wwise 2017.2.10.6745
   WWISE_SDK_VERSION=2017.2.10.6745
   WWISE_VC_VERSION=vc150
   ```

The project is configured to use **vc150** libraries exclusively, which are located at:
- `C:\Program Files (x86)\Audiokinetic\Wwise 2017.2.10.6745\SDK\x64_vc150\Release\lib`
- `C:\Program Files (x86)\Audiokinetic\Wwise 2017.2.10.6745\SDK\x64_vc150\Debug\lib`

## Building

### Using the Build Script (Easy)

#### Visual Studio / MSVC

1. Edit `.env` to set your Wwise SDK path
2. Run:
   ```cmd
   build.bat
   ```
   Or PowerShell:
   ```powershell
   .\build.ps1
   ```

#### MinGW / GCC

1. Install MinGW-w64 or MSYS2 with g++
   - [MSYS2](https://www.msys2.org/) (recommended)
   - [MinGW-w64](https://www.mingw-w64.org/)

2. Edit `.env` to set your Wwise SDK path

3. Run:
   ```cmd
   build-gcc.bat
   ```
   Or PowerShell:
   ```powershell
   .\build-gcc.ps1
   ```
   Or Bash:
   ```bash
   ./build-gcc.sh
   ```

#### Clang / LLVM

1. Install LLVM/Clang and Ninja build system
   - [LLVM](https://releases.llvm.org/)
   - Or use Visual Studio's "C++ Clang tools for Windows"
   - Install Ninja: `winget install Ninja-build.Ninja`

2. Edit `.env` to set your Wwise SDK path

3. Run:
   ```cmd
   build-clang.bat
   ```
   Or PowerShell:
   ```powershell
   .\build-clang.ps1
   ```
   Or Bash:
   ```bash
   ./build-clang.sh
   ```

### Manual Build

#### MSVC Build

1. Create build directory:
   ```cmd
   mkdir build
   cd build
   ```

2. Configure with CMake:
   ```cmd
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. Build:
   ```cmd
   cmake --build . --config Release
   ```

#### MinGW/GCC Build

1. Create build directory:
   ```bash
   mkdir build-gcc
   cd build-gcc
   ```

2. Configure with CMake:
   ```bash
   cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   ```

3. Build:
   ```bash
   cmake --build . --config Release
   ```

#### Clang Build

1. Create build directory:
   ```bash
   mkdir build-clang
   cd build-clang
   ```

2. Configure with CMake:
   ```bash
   cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
   ```

3. Build:
   ```bash
   cmake --build . --config Release
   ```

## Output

The compiled DLL and executables will be located at:

**MSVC:**
```
build/bin/Release/WallyMusicJukebox.dll
build/bin/Release/TestLoader.exe
build/bin/Release/InteractivePlayer.exe
```

**MinGW/GCC:**
```
build-gcc/bin/Release/libWallyMusicJukebox.dll
build-gcc/bin/Release/TestLoader.exe
build-gcc/bin/Release/InteractivePlayer.exe
```

**Clang:**
```
build-clang/bin/Release/libWallyMusicJukebox.dll
build-clang/bin/Release/TestLoader.exe
build-clang/bin/Release/InteractivePlayer.exe
```

## Testing

The project includes two test programs to verify functionality:

### TestLoader - DLL Dynamic Loading Test

Tests the DLL by loading it dynamically at runtime (using `LoadLibrary` and `GetProcAddress`):

```cmd
cd build\bin\Release
TestLoader.exe
```

The test loader will:
1. Dynamically load WallyMusicJukebox.dll
2. Initialize the Wwise sound engine with Brawlhalla's default settings
3. Load Init.bnk and VOX_Announcer.bnk automatically
4. Set up game objects (sound emitter, music emitter, listener)
5. Load and play the menu theme music (MUS_Menu_Theme_Play)
6. Wait for user input before cleaning up

**Note:** Requires WallyMusicJukebox.dll to be in the same directory.

### InteractivePlayer - Interactive Event Testing (Statically Linked)

The InteractivePlayer is a **statically-linked monolithic executable** that allows you to test any Wwise event interactively with on-demand bank loading. Unlike TestLoader, it compiles the entire Wwise implementation directly into the executable - no DLL required!

```cmd
cd build\bin\Release

# Use default Brawlhalla path
InteractivePlayer.exe

# Or specify custom audio path
InteractivePlayer.exe "C:\Custom\Path\To\Audio"
```

Features:
- **Statically linked** - No external DLL required, fully self-contained
- **On-demand bank loading** - Loads banks only when needed (solves memory issues)
- **Automatic bank management** - Unloads previous bank when loading new one
- **Bank::Event format** - Specify which bank to load with each event
- **Interactive event playback** - Test any Wwise event from any bank
- **Continuous audio rendering** - Audio engine runs in background thread
- **Emitter switching** - Choose between sound and music emitters
- **Playback control** - Stop events with fade-out and bank unloading

Commands:
- **Bank::Event format** - `BankName::EventName` (e.g., `MUS_Menu::MUS_Menu_Theme_Play`)
- `stop` - Stop current playback and unload current bank
- `music` - Switch to music emitter for next events
- `sound` - Switch to sound emitter for next events
- `quit` or `exit` - Exit the program

Example session:
```
[Music Emitter] Bank::Event: MUS_Menu::MUS_Menu_Theme_Play
Loading bank: MUS_Menu.bnk...
Bank loaded successfully!
Posting event: MUS_Menu_Theme_Play on Music emitter...
Event posted successfully! (Playing ID: 12345)

[Music Emitter] Bank::Event: SPC_Hattori::SPC_Hattori_VO_Reveal
Stopping current playback...
Unloading bank: MUS_Menu.bnk...
Loading bank: SPC_Hattori.bnk...
Bank loaded successfully!
Posting event: SPC_Hattori_VO_Reveal on Music emitter...
Event posted successfully! (Playing ID: 12346)

[Music Emitter] Bank::Event: stop
Stopping playback (ID: 12346)...
Unloading bank: SPC_Hattori.bnk...

[Music Emitter] Bank::Event: sound
Switched to Sound emitter.

[Sound Emitter] Bank::Event: UI::UI_Button_Click
Loading bank: UI.bnk...
Bank loaded successfully!
Posting event: UI_Button_Click on Sound emitter...
Event posted successfully! (Playing ID: 12347)

[Sound Emitter] Bank::Event: quit
Exiting...
```

**How it works:**
- **Statically linked** - Contains the entire Wwise implementation, no DLL needed
- Only loads one bank at a time (plus Init.bnk which is always loaded)
- Automatically unloads the previous bank when loading a new one
- Prevents memory issues by not loading all 212+ banks simultaneously
- Skips Init.bnk if requested (already loaded during initialization)

**Why static linking?**
- Self-contained executable - perfect for testing and debugging
- No DLL dependencies to manage
- Easier to distribute as a standalone tool
- Direct function calls without GetProcAddress overhead

**Note:** Brawlhalla must be installed at the default Steam location for the test to work:
`C:\Program Files (x86)\Steam\steamapps\common\Brawlhalla\audio\pc`

## Project Structure

```
WallyMusicJukebox/
├── include/                      # Public header files
│   ├── exports.hpp               # DLL export macros
│   └── WwiseManager.hpp          # Wwise manager C API interface
├── src/                          # Library source files
│   ├── main.cpp                  # DLL entry point
│   └── WwiseManager.cpp          # Wwise manager implementation
├── test/                         # Test programs
│   ├── test_loader.cpp           # DLL dynamic loading test
│   └── interactive_player.cpp    # Interactive event player (statically linked)
├── build/                        # MSVC build output (generated)
│   ├── bin/Release/              # Release binaries
│   │   ├── WallyMusicJukebox.dll
│   │   ├── TestLoader.exe
│   │   └── InteractivePlayer.exe
│   └── bin/Debug/                # Debug binaries
├── build-gcc/                    # GCC build output (generated)
├── build-clang/                  # Clang build output (generated)
├── .env                          # Build configuration (git-ignored)
├── .env.example                  # Example configuration template
├── .clangd                       # clangd language server config
├── .vscode/                      # VS Code settings
│   └── settings.json             # Editor configuration
├── CMakeLists.txt                # CMake build configuration
├── compile_flags.txt             # clangd compilation flags
├── build_config.json             # Additional build settings
├── build.bat/ps1/sh              # MSVC build scripts
├── build-gcc.bat/ps1/sh          # GCC build scripts
├── build-clang.bat/ps1/sh        # Clang build scripts
└── README.md                     # This file
```

## Usage

This DLL provides a C API compatible with Brawlhalla's SoundEngineExtension interface.

### Quick Start with Brawlhalla

The easiest way to use this DLL with Brawlhalla is to use `InitWithBrawlhallaDefaults()`:

```cpp
#include <Windows.h>
#include <iostream>
#include <cstdint>

int main() {
    // Load the DLL
    HMODULE hDLL = LoadLibraryA("WallyMusicJukebox.dll");
    if (hDLL == nullptr) {
        std::cerr << "Failed to load DLL!\n";
        return 1;
    }

    // Define function pointer types
    using InitWithDefaultsFunc = uint32_t (*)(const char*);
    using DisposeFunc = void (*)();
    using LoadBankFunc = uint32_t (*)(const char*);
    using RegisterGameObjFunc = uint32_t (*)(uint32_t, const char*);
    using SetDefaultListenerFunc = uint32_t (*)(uint32_t);
    using SetPositionFunc = uint32_t (*)(uint32_t, float, float, float, float, float, float);
    using SetRtpcValueFunc = uint32_t (*)(const char*, float, uint32_t);
    using PostEventFunc = uint32_t (*)(const char*, uint32_t);
    using TickSoundEngineExtensionFunc = uint32_t (*)();

    // Get function pointers
    auto InitWithDefaults = (InitWithDefaultsFunc)GetProcAddress(hDLL, "InitWithBrawlhallaDefaults");
    auto Dispose = (DisposeFunc)GetProcAddress(hDLL, "Dispose");
    auto LoadBank = (LoadBankFunc)GetProcAddress(hDLL, "LoadBank");
    auto RegisterGameObj = (RegisterGameObjFunc)GetProcAddress(hDLL, "RegisterGameObj");
    auto SetDefaultListener = (SetDefaultListenerFunc)GetProcAddress(hDLL, "SetDefaultListener");
    auto SetPosition = (SetPositionFunc)GetProcAddress(hDLL, "SetPosition");
    auto SetRtpcValue = (SetRtpcValueFunc)GetProcAddress(hDLL, "SetRtpcValue");
    auto PostEvent = (PostEventFunc)GetProcAddress(hDLL, "PostEvent");
    auto TickSoundEngineExtension = (TickSoundEngineExtensionFunc)GetProcAddress(hDLL, "TickSoundEngineExtension");

    // Check if all functions loaded successfully
    if (!InitWithDefaults || !Dispose || !LoadBank || !RegisterGameObj || 
        !SetDefaultListener || !SetPosition || !SetRtpcValue || !PostEvent || !TickSoundEngineExtension) {
        std::cerr << "Failed to load required functions!\n";
        FreeLibrary(hDLL);
        return 1;
    }

    // Initialize with Brawlhalla defaults (nullptr = use default path)
    // This automatically loads Init.bnk and VOX_Announcer.bnk
    if (InitWithDefaults(nullptr) != 1) {
        std::cerr << "Failed to initialize sound engine!\n";
        FreeLibrary(hDLL);
        return 1;
    }

    // Register game objects
    const uint32_t SOUND_EMITTER = 1;
    const uint32_t MUSIC_EMITTER = 2;
    const uint32_t LISTENER = 0;

    RegisterGameObj(SOUND_EMITTER, "Volume_Sound");
    RegisterGameObj(MUSIC_EMITTER, "Volume_Music");
    RegisterGameObj(LISTENER, "Listener");
    SetDefaultListener(LISTENER);

    // Set positions (position + orientation vector)
    SetPosition(LISTENER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
    SetPosition(SOUND_EMITTER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
    SetPosition(MUSIC_EMITTER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

    // Set volume RTPCs (Real-Time Parameter Controls)
    SetRtpcValue("Volume_Sound", 6.0f, SOUND_EMITTER);
    SetRtpcValue("Volume_Music", 6.0f, MUSIC_EMITTER);

    // Load and play menu music
    if (LoadBank("MUS_Menu.bnk") == 1) {
        uint32_t playingID = PostEvent("MUS_Menu_Theme_Play", MUSIC_EMITTER);
        if (playingID != 0) {
            std::cout << "Music playing! (ID: " << playingID << ")\n";
        }
    }

    // Game loop - must call TickSoundEngineExtension() every frame to process audio
    bool running = true;
    while (running) {
        TickSoundEngineExtension();  // Process audio engine
        
        // Your game logic here...
        // Check for exit condition
    }

    // Cleanup
    Dispose();
    FreeLibrary(hDLL);
    return 0;
}
```

### Manual Initialization

For more control, use the standard `Init()` function instead of `InitWithBrawlhallaDefaults()`:

```cpp
// Get Init function pointer
using InitFunc = uint32_t (*)(const char*);
auto Init = (InitFunc)GetProcAddress(hDLL, "Init");

// Initialize with custom path
if (Init("C:/Custom/Path/To/Audio") != 1) {
    std::cerr << "Failed to initialize!\n";
    return 1;
}

// Manually load required banks
if (LoadBank("Init.bnk") != 1) {
    std::cerr << "Failed to load Init.bnk!\n";
    return 1;
}

// Load localized voice bank
if (LoadBank("English(US)/VOX_Announcer.bnk") != 1) {
    std::cerr << "Failed to load VOX_Announcer.bnk!\n";
    return 1;
}

// Continue with game object setup...
RegisterGameObj(MUSIC_EMITTER, "Volume_Music");
// ... rest of setup
```

**Note:** `InitWithBrawlhallaDefaults()` automatically:
- Sets the base path to Brawlhalla's default location (if nullptr is passed)
- Loads `Init.bnk` (required by Wwise)
- Loads `English(US)/VOX_Announcer.bnk` (voice lines)

With manual `Init()`, you must load these banks yourself.

### Complete API Reference

| Function | Signature | Returns |
|----------|-----------|---------|
| `Init` | `uint32_t Init(const char* basePath)` | `1` on success, `0` on failure |
| `InitWithBrawlhallaDefaults` | `uint32_t InitWithBrawlhallaDefaults(const char* basePath)` | `1` on success, `0` on failure |
| `Dispose` | `void Dispose()` | void |
| `SetBasePath` | `uint32_t SetBasePath(const char* path)` | `1` on success, `0` on failure |
| `LoadBank` | `uint32_t LoadBank(const char* bank)` | `1` on success, `0` on failure |
| `UnloadBank` | `uint32_t UnloadBank(const char* bank)` | `1` on success, `0` on failure |
| `RegisterGameObj` | `uint32_t RegisterGameObj(uint32_t gameObj, const char* name)` | `1` on success, `0` on failure |
| `UnregisterGameObj` | `uint32_t UnregisterGameObj(uint32_t gameObj)` | `1` on success, `0` on failure |
| `SetDefaultListeners` | `uint32_t SetDefaultListeners(const uint32_t* gameObjs, uint32_t length)` | `1` on success, `0` on failure |
| `SetDefaultListener` | `uint32_t SetDefaultListener(uint32_t gameObj)` | `1` on success, `0` on failure |
| `StopPlaying` | `void StopPlaying(uint32_t playingID, int32_t fadeDuration)` | void |
| `PostEvent` | `uint32_t PostEvent(const char* event, uint32_t gameObj)` | Playing ID (`0` on failure) |
| `SetRtpcValue` | `uint32_t SetRtpcValue(const char* gameParam, float value, uint32_t gameObj)` | `1` on success, `0` on failure |
| `SetRtpcValueByPlayingID` | `uint32_t SetRtpcValueByPlayingID(const char* gameParam, float value, uint32_t playingID)` | `1` on success, `0` on failure |
| `SetPosition` | `uint32_t SetPosition(uint32_t gameObj, float posX, float posY, float posZ, float orientX, float orientY, float orientZ)` | `1` on success, `0` on failure |
| `SetListenerPosition` | `uint32_t SetListenerPosition(float posX, float posY, float posZ, float frontX, float frontY, float frontZ, float topX, float topY, float topZ)` | `1` on success, `0` on failure |
| `TickSoundEngineExtension` | `uint32_t TickSoundEngineExtension()` | `1` on success, `0` on failure |

### Example: 3D Positional Audio

```cpp
// Register game objects
uint32_t playerID = 1;
uint32_t listenerID = 100;

RegisterGameObj(playerID, "Player");
RegisterGameObj(listenerID, "MainListener");
SetDefaultListener(listenerID);

// Set listener position and orientation
// SetListenerPosition(posX, posY, posZ, frontX, frontY, frontZ, topX, topY, topZ)
SetListenerPosition(
    0.0f, 0.0f, 0.0f,      // Position (x, y, z)
    0.0f, 0.0f, 1.0f,      // Front vector (looking forward along Z)
    0.0f, 1.0f, 0.0f       // Top vector (Y is up)
);

// Set sound source position and orientation
// SetPosition(gameObj, posX, posY, posZ, orientX, orientY, orientZ)
SetPosition(playerID, 10.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f);

// Play positional sound
uint32_t playingID = PostEvent("Play_Footsteps", playerID);
```

### Example: Dynamic Music with RTPC

```cpp
// Setup music emitter
uint32_t musicEmitter = 2;
RegisterGameObj(musicEmitter, "Volume_Music");

// Start music
uint32_t musicID = PostEvent("Play_MainTheme", musicEmitter);

// Change music intensity based on gameplay
float intensity = 0.75f;  // 0.0 to 1.0
SetRtpcValue("MusicIntensity", intensity, musicEmitter);

// Or change parameter for specific playing sound by its ID
SetRtpcValueByPlayingID("Volume_Music", 0.5f, musicID);
```

## Troubleshooting

### CMake Cannot Find Wwise SDK

- Verify your `.env` file has the correct `WWISE_SDK_PATH`
- Ensure you have Wwise SDK version **2017.2.10.6745** installed (exact version required)
- Check that the path format uses forward slashes: `C:/Program Files (x86)/...`

### Linking Errors

- Verify the architecture (x64 vs Win32) matches between your project and Wwise SDK
- This project requires **vc150** libraries - ensure your Wwise SDK has the `x64_vc150` directory
- The project is hardcoded to use vc150 regardless of your Visual Studio version
- Ensure all required Wwise libraries are installed in the SDK directory
- Common library locations:
  - Release: `SDK/x64_vc150/Release/lib`
  - Debug: `SDK/x64_vc150/Debug/lib`

### Runtime Errors

- Make sure Wwise runtime DLLs are in your PATH or next to your executable
- Verify sound banks are accessible from your working directory
- Check that the Wwise project is properly configured
- Ensure you call `Init()` before using any other functions
- Call `TickSoundEngineExtension()` every frame for audio processing

### Testing the DLL

Run the included test loader to verify the DLL works:

```cmd
cd build\bin\Release
TestLoader.exe
```

The test loader will:
1. Load the DLL
2. Call all major API functions
3. Display results for each call
4. Wait for input before cleanup

### IntelliSense Issues

- If clangd isn't found, install LLVM: `winget install LLVM.LLVM`
- Reload VS Code window after installing clangd
- Check that include paths in `.clangd` and `compile_flags.txt` are correct

## License

This project is licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0).

This means you can:
- Use this library in both open source and proprietary software
- Link this library dynamically without releasing your application's source code
- Modify and redistribute the library under the same LGPL-3.0 license

See the [LICENSE](LICENSE) file for full details.
