@echo off
REM Build script for WallyMusicJukebox DLL using Clang

echo ====================================
echo Building WallyMusicJukebox DLL (Clang)
echo ====================================

REM Load environment variables from .env file
if exist .env (
    echo Loading configuration from .env...
    for /f "usebackq tokens=1,* delims==" %%a in (".env") do (
        set "%%a=%%b"
    )
) else (
    echo Warning: .env file not found, using defaults
    echo Copy .env.example to .env and configure your paths
    set "WWISE_SDK_PATH=C:/Program Files (x86)/Audiokinetic/Wwise"
    set "WWISE_SDK_VERSION=2017.2.10.6745"
    set "BUILD_CONFIG=Release"
)

REM Override generator for Clang build
set "CMAKE_GENERATOR=Ninja"

REM Check for clang++
where clang++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: clang++ not found in PATH
    echo Please install LLVM/Clang or use Visual Studio's clang-cl
    echo   LLVM: https://releases.llvm.org/
    echo   Or install "C++ Clang tools for Windows" in Visual Studio
    pause
    exit /b 1
)

REM Check for Ninja
where ninja >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Ninja build system not found
    echo Please install Ninja: https://ninja-build.org/
    echo Or use: winget install Ninja-build.Ninja
    pause
    exit /b 1
)

REM Display configuration
echo.
echo Build Configuration:
for /f "tokens=*" %%i in ('clang++ --version ^| findstr /r "^clang"') do echo   Compiler: %%i
echo   Wwise SDK: %WWISE_SDK_PATH%
echo   SDK Version: %WWISE_SDK_VERSION%
echo   Generator: %CMAKE_GENERATOR%
echo   Config: %BUILD_CONFIG%
echo.

REM Create build directory
set "BUILD_DIR=build-clang"
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Run CMake configuration
echo Configuring CMake...
cmake .. -G "%CMAKE_GENERATOR%" ^
    -DCMAKE_BUILD_TYPE=%BUILD_CONFIG% ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_C_COMPILER=clang ^
    -DWWISE_SDK_PATH="%WWISE_SDK_PATH%" ^
    -DWWISE_SDK_VERSION="%WWISE_SDK_VERSION%"

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build . --config %BUILD_CONFIG%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ====================================
echo Build completed successfully!
echo Output: %BUILD_DIR%\bin\%BUILD_CONFIG%\libWallyMusicJukebox.dll
echo ====================================

cd ..
pause
