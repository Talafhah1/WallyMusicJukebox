#include <Windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <libloaderapi.h>
#include <minwindef.h>

int main() {
    std::cout << "WallyMusicJukebox DLL Test Loader\n";
    std::cout << "==================================\n\n";

    // Get the DLL path (assuming it's in the same directory or in bin/Release)
    const char* dllName = "WallyMusicJukebox.dll";
    
    std::cout << "Loading " << dllName << "...\n";
    
    // Load the DLL
    HMODULE hModule = LoadLibraryA(dllName);
    
    if (hModule == nullptr) {
        DWORD error = GetLastError();
        std::cerr << "Failed to load DLL! Error code: " << error << "\n";
        std::cerr << "Make sure " << dllName << " is in the same directory as this executable.\n";
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "DLL loaded successfully!\n";
    std::cout << "Module handle: 0x" << std::hex << hModule << std::dec << "\n\n";
    
    // Keep the program running
    std::cout << "Press Enter to unload DLL and exit...";
    std::cin.get();
    
    // Unload the DLL
    std::cout << "\nUnloading DLL...\n";
    FreeLibrary(hModule);
    std::cout << "DLL unloaded. Exiting.\n";
    
    return 0;
}
