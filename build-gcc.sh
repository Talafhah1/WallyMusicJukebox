#!/bin/bash
# Bash build script for WallyMusicJukebox DLL using g++/MinGW

set -e  # Exit on error

echo "===================================="
echo "Building WallyMusicJukebox DLL (g++)"
echo "===================================="

# Load environment variables from .env file
if [ -f ".env" ]; then
    echo "Loading configuration from .env..."
    export $(grep -v '^#' .env | grep -v '^[[:space:]]*$' | xargs)
else
    echo "Warning: .env file not found, using defaults"
    echo "Copy .env.example to .env and configure your paths"
    export WWISE_SDK_PATH="C:/Program Files (x86)/Audiokinetic/Wwise"
    export WWISE_SDK_VERSION="2017.2.10.6745"
    export CMAKE_GENERATOR="MinGW Makefiles"
    export BUILD_CONFIG="Release"
fi

# Override generator for g++ build
export CMAKE_GENERATOR="MinGW Makefiles"

# Check for g++
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found in PATH"
    echo "Please install MinGW-w64 or MSYS2 with g++"
    echo "  MSYS2: https://www.msys2.org/"
    echo "  MinGW-w64: https://www.mingw-w64.org/"
    exit 1
fi

# Display configuration
echo ""
echo "Build Configuration:"
echo "  Compiler: $(g++ --version | head -n1)"
echo "  Wwise SDK: $WWISE_SDK_PATH"
echo "  SDK Version: $WWISE_SDK_VERSION"
echo "  Generator: $CMAKE_GENERATOR"
echo "  Config: $BUILD_CONFIG"
echo ""

# Create build directory
BUILD_DIR="build-gcc"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake configuration
echo "Configuring CMake..."
cmake .. -G "$CMAKE_GENERATOR" \
    -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc \
    -DWWISE_SDK_PATH="$WWISE_SDK_PATH" \
    -DWWISE_SDK_VERSION="$WWISE_SDK_VERSION"

# Build the project
echo ""
echo "Building project..."
cmake --build . --config "$BUILD_CONFIG" -- -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "===================================="
echo "Build completed successfully!"
echo "Output: $BUILD_DIR/bin/$BUILD_CONFIG/libWallyMusicJukebox.dll"
echo "===================================="

cd ..
