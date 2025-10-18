cmake_minimum_required(VERSION 3.20)
project(WallyMusicJukebox VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Wwise SDK version - MUST be exactly this version for Brawlhalla compatibility
set(WWISE_SDK_VERSION "2017.2.10.6745" CACHE STRING "Wwise SDK Version" FORCE)

# Wwise SDK Path - Full path including version
set(WWISE_SDK_PATH "C:/Program Files (x86)/Audiokinetic/Wwise 2017.2.10.6745" CACHE PATH "Path to Wwise SDK installation")

# Platform detection
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WWISE_PLATFORM "x64")
        set(WWISE_ARCH "x64")
    else()
        set(WWISE_PLATFORM "Win32")
        set(WWISE_ARCH "x86")
    endif()
    
    # Force vc150 for Wwise 2017.2.10.6745 compatibility
    set(WWISE_VC "vc150")
    message(STATUS "Using Wwise libraries built with ${WWISE_VC}")
endif()

# Wwise SDK directories
set(WWISE_SDK_ROOT "${WWISE_SDK_PATH}/SDK")
set(WWISE_INCLUDE_DIR "${WWISE_SDK_ROOT}/include")
set(WWISE_LIB_DIR "${WWISE_SDK_ROOT}/${WWISE_ARCH}_${WWISE_VC}/Release/lib")
set(WWISE_LIB_DIR_DEBUG "${WWISE_SDK_ROOT}/${WWISE_ARCH}_${WWISE_VC}/Debug/lib")

# Verify Wwise SDK exists
if(NOT EXISTS "${WWISE_SDK_ROOT}")
    message(WARNING "Wwise SDK not found at: ${WWISE_SDK_ROOT}")
    message(WARNING "Please set WWISE_SDK_PATH and WWISE_SDK_VERSION correctly")
    message(WARNING "You can set WWISEROOT environment variable or pass -DWWISE_SDK_PATH=<path> to CMake")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/WwiseManager.cpp
)

set(HEADERS
    include/WwiseManager.hpp
    include/exports.hpp
)

# Wwise SDK sample implementation files
set(WWISE_SAMPLES_COMMON
    "${WWISE_SDK_ROOT}/samples/SoundEngine/Common/AkFileLocationBase.cpp"
    "${WWISE_SDK_ROOT}/samples/SoundEngine/Common/AkFilePackage.cpp"
    "${WWISE_SDK_ROOT}/samples/SoundEngine/Common/AkFilePackageLUT.cpp"
)

set(WWISE_SAMPLES_WIN32
    "${WWISE_SDK_ROOT}/samples/SoundEngine/Win32/AkDefaultIOHookBlocking.cpp"
)

# Create DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${WWISE_SAMPLES_COMMON} ${WWISE_SAMPLES_WIN32})

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${WWISE_INCLUDE_DIR}
        ${WWISE_SDK_ROOT}/samples/SoundEngine/Win32
        ${WWISE_SDK_ROOT}/samples/SoundEngine/Common
)

# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        WALLYMUSICJUKEBOX_EXPORTS
        $<$<PLATFORM_ID:Windows>:_UNICODE;UNICODE>
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4                 # Warning level 4
        /MP                 # Multi-processor compilation
        /permissive-        # Standards conformance
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall               # Enable all warnings
        -Wextra             # Extra warnings
        -pedantic           # Strict ISO C++
        -fvisibility=hidden # Hide symbols by default
    )
endif()

# Link directories
target_link_directories(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:${WWISE_LIB_DIR_DEBUG}>
        $<$<CONFIG:Release>:${WWISE_LIB_DIR}>
)

# Wwise libraries to link (common ones)
set(WWISE_LIBS
    AkSoundEngine
    AkMemoryMgr
    AkStreamMgr
    AkMusicEngine
    AkSpatialAudio
    AkVorbisDecoder
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${WWISE_LIBS}
        # Windows libraries
        ws2_32
        dinput8
        dxguid
        dsound
        winmm
        # Note: dxerr.lib is deprecated and removed from modern Windows SDKs
)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Copy DLL to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${PROJECT_NAME}.dll
    COMMENT "Copying DLL to output directory"
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include
)

# Test executable
add_executable(TestLoader test/test_loader.cpp)

# Test executable output to same directory as DLL
set_target_properties(TestLoader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy test executable next to DLL
add_custom_command(TARGET TestLoader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:TestLoader>
        ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/TestLoader.exe
    COMMENT "Copying test executable to output directory"
)

# Interactive Player - Statically linked monolithic executable
add_executable(InteractivePlayer 
    test/interactive_player.cpp
    src/WwiseManager.cpp
    ${WWISE_SAMPLES_COMMON}
    ${WWISE_SAMPLES_WIN32}
)

# Include directories for interactive player
target_include_directories(InteractivePlayer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${WWISE_INCLUDE_DIR}
        ${WWISE_SDK_ROOT}/samples/SoundEngine/Win32
        ${WWISE_SDK_ROOT}/samples/SoundEngine/Common
)

# Preprocessor definitions for interactive player
target_compile_definitions(InteractivePlayer
    PRIVATE
        WALLYMUSICJUKEBOX_EXPORTS
        $<$<PLATFORM_ID:Windows>:_UNICODE;UNICODE>
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

# Link directories for interactive player
target_link_directories(InteractivePlayer
    PRIVATE
        $<$<CONFIG:Debug>:${WWISE_LIB_DIR_DEBUG}>
        $<$<CONFIG:Release>:${WWISE_LIB_DIR}>
)

# Link libraries for interactive player
target_link_libraries(InteractivePlayer
    PRIVATE
        ${WWISE_LIBS}
        # Windows libraries
        ws2_32
        dinput8
        dxguid
        dsound
        winmm
)

# Set output properties for interactive player
set_target_properties(InteractivePlayer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy interactive player to output directory
add_custom_command(TARGET InteractivePlayer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:InteractivePlayer>
        ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/InteractivePlayer.exe
    COMMENT "Copying interactive player to output directory"
)
