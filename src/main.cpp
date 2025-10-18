#include <Windows.h>
#include <minwindef.h>
#include <winnt.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void)hModule;
    (void)lpReserved;
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // DLL is being loaded into the virtual address space
        break;
    case DLL_THREAD_ATTACH:
        // A new thread is being created
        break;
    case DLL_THREAD_DETACH:
        // A thread is exiting cleanly
        break;
    case DLL_PROCESS_DETACH:
        // DLL is being unloaded
        break;
    }
    return TRUE;
}
