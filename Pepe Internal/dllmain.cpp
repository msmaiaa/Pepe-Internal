#include <iostream>
#include "csgosdk.h"
#include "entity.h"

uintptr_t DETACH_KEY = VK_END;

DWORD WINAPI InternalMain(HMODULE hMod) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif

    IClientEntityList* ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
    LocalPlayer* localPlayer = (LocalPlayer*)ClientEntityList->GetClientEntity(1);
    while (!GetAsyncKeyState(DETACH_KEY) & 1) {
        localPlayer->resetFlashDuration();
        Sleep(10);
    }

#ifdef _DEBUG //Close our console
    if (f != nullptr) fclose(f);
    FreeConsole();
#endif 
    //eject our library
    FreeLibraryAndExitThread(hMod, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        HANDLE tHndl = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InternalMain, hModule, 0, 0);
        if (tHndl) CloseHandle(tHndl);
        else return FALSE;
        break;
    }
    return TRUE;
}
