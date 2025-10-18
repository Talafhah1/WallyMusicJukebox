#!/bin/bash
# Bash build script for WallyMusicJukebox DLL using Clang

set -e  # Exit on error

echo "===================================="
echo "Building WallyMusicJukebox DLL (Clang)"
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
    export BUILD_CONFIG="Release"
fi

# Override generator for Clang build
export CMAKE_GENERATOR="Ninja"

# Check for clang++
if ! command -v clang++ &> /dev/null; then
    echo "Error: clang++ not found in PATH"
    echo "Please install LLVM/Clang"
    echo "  LLVM: https://releases.llvm.org/"
    exit 1
fi

# Check for Ninja
if ! command -v ninja &> /dev/null; then
    echo "Error: Ninja build system not found"
    echo "Please install Ninja: https://ninja-build.org/"
    exit 1
fi

# Display configuration
echo ""
echo "Build Configuration:"
echo "  Compiler: $(clang++ --version | head -n1)"
echo "  Wwise SDK: $WWISE_SDK_PATH"
echo "  SDK Version: $WWISE_SDK_VERSION"
echo "  Generator: $CMAKE_GENERATOR"
echo "  Config: $BUILD_CONFIG"
echo ""

# Create build directory
BUILD_DIR="build-clang"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake configuration
echo "Configuring CMake..."
cmake .. -G "$CMAKE_GENERATOR" \
    -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
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
