@echo off
REM Build script for WallyMusicJukebox DLL

echo ====================================
echo Building WallyMusicJukebox DLL
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
    set "CMAKE_GENERATOR=Visual Studio 17 2022"
    set "BUILD_ARCH=x64"
    set "BUILD_CONFIG=Release"
)

REM Display configuration
echo.
echo Build Configuration:
echo   Wwise SDK: %WWISE_SDK_PATH%
echo   SDK Version: %WWISE_SDK_VERSION%
echo   Generator: %CMAKE_GENERATOR%
echo   Architecture: %BUILD_ARCH%
echo   Config: %BUILD_CONFIG%
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake configuration
echo Configuring CMake...
cmake .. -G "%CMAKE_GENERATOR%" -A %BUILD_ARCH% -DWWISE_SDK_PATH="%WWISE_SDK_PATH%" -DWWISE_SDK_VERSION="%WWISE_SDK_VERSION%"

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
echo Output: build\bin\%BUILD_CONFIG%\WallyMusicJukebox.dll
echo ====================================

cd ..
pause
