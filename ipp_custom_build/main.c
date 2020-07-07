#include <Windows.h>
#include "ipp.h"

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        ippInit(); break;
    case DLL_THREAD_ATTACH: break;
    case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH: break;
    default: break;
    }
    return 1;
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpvReserved);
}
