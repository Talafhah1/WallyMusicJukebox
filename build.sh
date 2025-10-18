#!/bin/bash
# Bash build script for WallyMusicJukebox DLL

set -e  # Exit on error

echo "===================================="
echo "Building WallyMusicJukebox DLL"
echo "===================================="

# Load environment variables from .env file
if [ -f ".env" ]; then
    echo "Loading configuration from .env..."
    export $(grep -v '^#' .env | xargs)
else
    echo "Warning: .env file not found, using defaults"
    echo "Copy .env.example to .env and configure your paths"
    export WWISE_SDK_PATH="C:/Program Files (x86)/Audiokinetic/Wwise"
    export WWISE_SDK_VERSION="2017.2.10.6745"
    export CMAKE_GENERATOR="Visual Studio 17 2022"
    export BUILD_ARCH="x64"
    export BUILD_CONFIG="Release"
fi

# Display configuration
echo ""
echo "Build Configuration:"
echo "  Wwise SDK: $WWISE_SDK_PATH"
echo "  SDK Version: $WWISE_SDK_VERSION"
echo "  Generator: $CMAKE_GENERATOR"
echo "  Architecture: $BUILD_ARCH"
echo "  Config: $BUILD_CONFIG"
echo ""

# Create build directory
mkdir -p build
cd build

# Run CMake configuration
echo "Configuring CMake..."
cmake .. -G "$CMAKE_GENERATOR" -A "$BUILD_ARCH" \
    -DWWISE_SDK_PATH="$WWISE_SDK_PATH" \
    -DWWISE_SDK_VERSION="$WWISE_SDK_VERSION"

# Build the project
echo ""
echo "Building project..."
cmake --build . --config "$BUILD_CONFIG"

echo ""
echo "===================================="
echo "Build completed successfully!"
echo "Output: build/bin/$BUILD_CONFIG/WallyMusicJukebox.dll"
echo "===================================="

cd ..
