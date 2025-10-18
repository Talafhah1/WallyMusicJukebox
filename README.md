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

The compiled DLL and test executable will be located at:

**MSVC:**
```
build/bin/Release/WallyMusicJukebox.dll
build/bin/Release/TestLoader.exe
```

**MinGW/GCC:**
```
build-gcc/bin/Release/libWallyMusicJukebox.dll
build-gcc/bin/Release/TestLoader.exe
```

**Clang:**
```
build-clang/bin/Release/libWallyMusicJukebox.dll
build-clang/bin/Release/TestLoader.exe
```

## Testing

After building, you can test the DLL with the included TestLoader executable:

```cmd
cd build\bin\Release
TestLoader.exe
```

The test loader will:
1. Initialize the Wwise sound engine with Brawlhalla's default settings
2. Load Init.bnk and VOX_Announcer.bnk automatically
3. Set up game objects (sound emitter, music emitter, listener)
4. Load and play the menu theme music (MUS_Menu_Theme_Play)
5. Wait for user input before cleaning up

**Note:** Brawlhalla must be installed at the default Steam location for the test to work:
`C:\Program Files (x86)\Steam\steamapps\common\Brawlhalla\audio\pc`

## Project Structure

```
WallyMusicJukebox/
├── include/               # Public header files
│   ├── exports.hpp        # DLL export macros
│   └── WwiseManager.hpp   # Wwise manager interface
├── src/                   # Source files
│   ├── main.cpp           # DLL entry point
│   └── WwiseManager.cpp   # Wwise manager implementation
├── build/                 # MSVC build output (generated)
├── build-gcc/             # GCC build output (generated)
├── build-clang/           # Clang build output (generated)
├── .env                   # Build configuration (git-ignored)
├── .env.example           # Example configuration template
├── .clangd                # clangd language server config
├── CMakeLists.txt         # CMake build configuration
├── build_config.json      # Additional build settings
├── build.bat/ps1/sh       # MSVC build scripts
├── build-gcc.bat/ps1/sh   # GCC build scripts
├── build-clang.bat/ps1/sh # Clang build scripts
└── README.md              # This file
```

## Usage

This DLL provides a C API compatible with Brawlhalla's SoundEngineExtension interface.

### Quick Start with Brawlhalla

The easiest way to use this DLL with Brawlhalla is to use `InitWithBrawlhallaDefaults()`:

```cpp
#include <Windows.h>
#include <iostream>

// Load the DLL
HMODULE hDLL = LoadLibraryA("WallyMusicJukebox.dll");

// Get function pointers
typedef uint32_t (*InitWithDefaultsFunc)(const char*);
typedef void (*DisposeFunc)();
typedef uint32_t (*LoadBankFunc)(const char*);
typedef uint32_t (*RegisterGameObjFunc)(uint32_t, const char*);
typedef uint32_t (*SetDefaultListenerFunc)(uint32_t);
typedef uint32_t (*SetPositionFunc)(uint32_t, float, float, float, float, float, float);
typedef uint32_t (*SetRtpcValueFunc)(const char*, float, uint32_t);
typedef uint32_t (*PostEventFunc)(const char*, uint32_t);
typedef uint32_t (*TickFunc)();

auto InitWithDefaults = (InitWithDefaultsFunc)GetProcAddress(hDLL, "InitWithBrawlhallaDefaults");
auto Dispose = (DisposeFunc)GetProcAddress(hDLL, "Dispose");
auto LoadBank = (LoadBankFunc)GetProcAddress(hDLL, "LoadBank");
auto RegisterGameObj = (RegisterGameObjFunc)GetProcAddress(hDLL, "RegisterGameObj");
auto SetDefaultListener = (SetDefaultListenerFunc)GetProcAddress(hDLL, "SetDefaultListener");
auto SetPosition = (SetPositionFunc)GetProcAddress(hDLL, "SetPosition");
auto SetRtpcValue = (SetRtpcValueFunc)GetProcAddress(hDLL, "SetRtpcValue");
auto PostEvent = (PostEventFunc)GetProcAddress(hDLL, "PostEvent");
auto Tick = (TickFunc)GetProcAddress(hDLL, "TickSoundEngineExtension");

// Initialize with Brawlhalla defaults (nullptr = use default path)
// This automatically loads Init.bnk and VOX_Announcer.bnk
InitWithDefaults(nullptr);

// Register game objects
const uint32_t SOUND_EMITTER = 1;
const uint32_t MUSIC_EMITTER = 2;
const uint32_t LISTENER = 0;

RegisterGameObj(SOUND_EMITTER, "Volume_Sound");
RegisterGameObj(MUSIC_EMITTER, "Volume_Music");
RegisterGameObj(LISTENER, "Listener");
SetDefaultListener(LISTENER);

// Set positions
SetPosition(LISTENER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
SetPosition(SOUND_EMITTER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
SetPosition(MUSIC_EMITTER, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

// Set volume RTPCs
SetRtpcValue("Volume_Sound", 6.0f, SOUND_EMITTER);
SetRtpcValue("Volume_Music", 6.0f, MUSIC_EMITTER);

// Load and play menu music
LoadBank("MUS_Menu.bnk");
PostEvent("MUS_Menu_Theme_Play", MUSIC_EMITTER);

// Game loop
while (running) {
    Tick();  // Call every frame to process audio
    // Your game logic...
}

// Cleanup
Dispose();
FreeLibrary(hDLL);
```

### Manual Initialization

For more control, use the standard `Init()` function:

```cpp
auto Init = (InitFunc)GetProcAddress(hDLL, "Init");

// Initialize with custom path
Init("C:/Custom/Path/To/Audio");

// Manually load banks
LoadBank("Init.bnk");
LoadBank("English(US)/VOX_Announcer.bnk");

// Continue with game setup...
```

### Complete API Reference

| Function | Description | Returns |
|----------|-------------|---------|
| `Init(basePath)` | Initialize sound engine with base path | `1` on success, `0` on failure |
| `Dispose()` | Shutdown and cleanup sound engine | void |
| `SetBasePath(path)` | Change the base path for sound banks | `1` on success, `0` on failure |
| `LoadBank(bank)` | Load a sound bank by name | `1` on success, `0` on failure |
| `UnloadBank(bank)` | Unload a sound bank by name | `1` on success, `0` on failure |
| `RegisterGameObj(id, name)` | Register a game object for audio | `1` on success, `0` on failure |
| `UnregisterGameObj(id)` | Unregister a game object | `1` on success, `0` on failure |
| `SetDefaultListener(id)` | Set the default audio listener | `1` on success, `0` on failure |
| `StopPlaying(id, fade)` | Stop a playing sound with fade (ms) | void |
| `PostEvent(event, gameObj)` | Post an audio event | Playing ID (0 on failure) |
| `SetRtpcValue(param, value, obj)` | Set real-time parameter | `1` on success, `0` on failure |
| `SetPosition(obj, x, y, z, ...)` | Set 3D position for game object | `1` on success, `0` on failure |
| `SetListenerPosition(x, y, z, ...)` | Set 3D listener position | `1` on success, `0` on failure |
| `TickSoundEngineExtension()` | Update sound engine (call per frame) | `1` on success, `0` on failure |

### Example: 3D Positional Audio

```cpp
// Register game objects
uint32_t playerID = 1;
uint32_t listenerID = 100;

RegisterGameObj(playerID, "Player");
RegisterGameObj(listenerID, "MainListener");
SetDefaultListener(listenerID);

// Set listener position and orientation
SetListenerPosition(
    0.0f, 0.0f, 0.0f,           // Position
    0.0f, 0.0f, 1.0f,           // Front orientation
    0.0f, 1.0f, 0.0f            // Top orientation
);

// Set sound source position
SetPosition(playerID, 10.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f);

// Play positional sound
PostEvent("Play_Footsteps", playerID);
```

### Example: Dynamic Music with RTPC

```cpp
// Start music
uint32_t musicID = PostEvent("Play_MainTheme", 0);

// Change music intensity based on gameplay
float intensity = 0.75f;  // 0.0 to 1.0
SetRtpcValue("MusicIntensity", intensity, 0);

// Or change parameter for specific playing sound
SetRtpcValueByPlayingID("Volume", 0.5f, musicID);
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
