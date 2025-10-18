#!/usr/bin/env pwsh
# PowerShell build script for WallyMusicJukebox DLL

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "Building WallyMusicJukebox DLL" -ForegroundColor Cyan
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
    $CMAKE_GENERATOR = "Visual Studio 17 2022"
    $BUILD_ARCH = "x64"
    $BUILD_CONFIG = "Release"
}

# Display configuration
Write-Host ""
Write-Host "Build Configuration:" -ForegroundColor Cyan
Write-Host "  Wwise SDK: $WWISE_SDK_PATH" -ForegroundColor White
Write-Host "  SDK Version: $WWISE_SDK_VERSION" -ForegroundColor White
Write-Host "  Generator: $CMAKE_GENERATOR" -ForegroundColor White
Write-Host "  Architecture: $BUILD_ARCH" -ForegroundColor White
Write-Host "  Config: $BUILD_CONFIG" -ForegroundColor White
Write-Host ""

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Push-Location "build"

try {
    # Run CMake configuration
    Write-Host "Configuring CMake..." -ForegroundColor Cyan
    cmake .. -G "$CMAKE_GENERATOR" -A $BUILD_ARCH `
        -DWWISE_SDK_PATH="$WWISE_SDK_PATH" `
        -DWWISE_SDK_VERSION="$WWISE_SDK_VERSION"
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed!"
    }

    # Build the project
    Write-Host ""
    Write-Host "Building project..." -ForegroundColor Cyan
    cmake --build . --config $BUILD_CONFIG
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed!"
    }

    Write-Host ""
    Write-Host "====================================" -ForegroundColor Green
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "Output: build\bin\$BUILD_CONFIG\WallyMusicJukebox.dll" -ForegroundColor Green
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
