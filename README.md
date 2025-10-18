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

The compiled DLL will be located at:

**MSVC:**
```
build/bin/Release/WallyMusicJukebox.dll
```

**MinGW/GCC:**
```
build-gcc/bin/Release/libWallyMusicJukebox.dll
```

**Clang:**
```
build-clang/bin/Release/libWallyMusicJukebox.dll
```

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

## Usage Example

### C++ Usage

```cpp
#include "WwiseManager.hpp"

WallyMusic::WwiseManager manager;
if (manager.Initialize()) {
    manager.LoadBank("Init.bnk");
    manager.LoadBank("MyMusicBank.bnk");
    manager.PostEvent("Play_Music");
    
    // In your game loop
    manager.Update();
    
    // Cleanup
    manager.Shutdown();
}
```

### C-Style API Usage

```c
void* manager = CreateWwiseManager();
if (InitializeWwise(manager)) {
    LoadBankWwise(manager, "Init.bnk");
    PostEventWwise(manager, "Play_Music");
    
    // In your game loop
    UpdateWwise(manager);
    
    // Cleanup
    ShutdownWwise(manager);
}
DestroyWwiseManager(manager);
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
