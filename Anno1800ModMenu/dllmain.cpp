#include "pch.h"
#include "d3d11hook.h"


DWORD WINAPI MainThread(HMODULE hModule)
{
    //Sleep(30000); 
    ImplHookDX11_Init(hModule, FindWindow(L"Anno 1800", L"Anno 1800"));
    return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
    }

    return TRUE; 
}

