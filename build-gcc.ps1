#!/usr/bin/env pwsh
# PowerShell build script for WallyMusicJukebox DLL using g++/MinGW

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "Building WallyMusicJukebox DLL (g++)" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan

# Load environment variables from .env file
if (Test-Path ".env") {
    Write-Host "Loading configuration from .env..." -ForegroundColor Green
    Get-Content ".env" | ForEach-Object {
        if ($_ -match '^\s*([^#][^=]+)=(.*)$') {
            $name = $matches[1].Trim()
            $value = $matches[2].Trim()
            Set-Variable -Name $name -Value $value -Scope Script
        }
    }
} else {
    Write-Host "Warning: .env file not found, using defaults" -ForegroundColor Yellow
    Write-Host "Copy .env.example to .env and configure your paths" -ForegroundColor Yellow
    $WWISE_SDK_PATH = "C:/Program Files (x86)/Audiokinetic/Wwise"
    $WWISE_SDK_VERSION = "2017.2.10.6745"
    $BUILD_CONFIG = "Release"
}

# Override generator for g++ build
$CMAKE_GENERATOR = "MinGW Makefiles"

# Check for g++
try {
    $gccVersion = & g++ --version 2>&1 | Select-Object -First 1
    if (-not $gccVersion) {
        throw "g++ not found"
    }
} catch {
    Write-Host "Error: g++ not found in PATH" -ForegroundColor Red
    Write-Host "Please install MinGW-w64 or MSYS2 with g++" -ForegroundColor Yellow
    Write-Host "  MSYS2: https://www.msys2.org/" -ForegroundColor Yellow
    Write-Host "  MinGW-w64: https://www.mingw-w64.org/" -ForegroundColor Yellow
    exit 1
}

# Display configuration
Write-Host ""
Write-Host "Build Configuration:" -ForegroundColor Cyan
Write-Host "  Compiler: $gccVersion" -ForegroundColor White
Write-Host "  Wwise SDK: $WWISE_SDK_PATH" -ForegroundColor White
Write-Host "  SDK Version: $WWISE_SDK_VERSION" -ForegroundColor White
Write-Host "  Generator: $CMAKE_GENERATOR" -ForegroundColor White
Write-Host "  Config: $BUILD_CONFIG" -ForegroundColor White
Write-Host ""

# Create build directory
$BUILD_DIR = "build-gcc"
if (-not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

Push-Location $BUILD_DIR

try {
    # Run CMake configuration
    Write-Host "Configuring CMake..." -ForegroundColor Cyan
    cmake .. -G "$CMAKE_GENERATOR" `
        -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" `
        -DCMAKE_CXX_COMPILER=g++ `
        -DCMAKE_C_COMPILER=gcc `
        -DWWISE_SDK_PATH="$WWISE_SDK_PATH" `
        -DWWISE_SDK_VERSION="$WWISE_SDK_VERSION"
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed!"
    }

    # Build the project
    Write-Host ""
    Write-Host "Building project..." -ForegroundColor Cyan
    $numCores = (Get-CimInstance -ClassName Win32_Processor).NumberOfCores
    cmake --build . --config $BUILD_CONFIG -- -j$numCores
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed!"
    }

    Write-Host ""
    Write-Host "====================================" -ForegroundColor Green
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "Output: $BUILD_DIR\bin\$BUILD_CONFIG\libWallyMusicJukebox.dll" -ForegroundColor Green
    Write-Host "====================================" -ForegroundColor Green
}
catch {
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
    Pop-Location
    exit 1
}
finally {
    Pop-Location
}
