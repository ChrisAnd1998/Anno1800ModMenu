// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

HMODULE hm;

DWORD WINAPI Init(LPVOID lpParameter){
    while (GetModuleHandle(L"d3d9.dll") == NULL) {
        Sleep(1);
    }
    while (GetModuleHandle(L"d3d11.dll") == NULL) {
        Sleep(1);
    }
    while (GetModuleHandle(L"d3d12.dll") == NULL) {
        Sleep(1);
    }

    Sleep(1000);

    //while (!GetAsyncKeyState(VK_F7) & 1) {
    //    Sleep(1);
    //}


    HANDLE isDirectX12 = GetModuleHandle(L"d3d11on12.dll");

    //MessageBox(NULL, (LPSTR)isDirectX12, "", MB_OK);

    if (isDirectX12 == NULL) {
        LoadLibrary(L"Anno1800ModMenuDX11.dll");
    }
    else {
        LoadLibrary(L"Anno1800ModMenuDX12.dll");
    }

    FreeLibraryAndExitThread(hm, 0);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        hm = hModule;
        CreateThread(NULL, 0, Init, NULL, 0, NULL);
    }


   
    return TRUE;
}

