#include <iostream>
#include "csgo.h"
#include "features.h"
#include "csgosdk.h"
#include "entity.h"

uintptr_t DETACH_KEY = VK_END;
uintptr_t BHOP_KEY = VK_SPACE;
bool noFlashActivated = false;
bool isRadarActivated = false;
bool isBhopActivated = false;
bool isGlowActivated = false;

LocalPlayer* localPlayer;
IClientEntityList* ClientEntityList;
uintptr_t clientModule;
uintptr_t* glowObject;

void printMenu() {
    std::cout << "[F1] No flash\n[F2] Radar\n[F3] Bunnyhop\n[F4] Glowhack\n[F5] RCS" << std::endl;
}

DWORD WINAPI InternalMain(HMODULE hMod) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
    printMenu();
    ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
    localPlayer = (LocalPlayer*)ClientEntityList->GetClientEntity(1);
    clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
    glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);

    while (!GetAsyncKeyState(DETACH_KEY) & 1) {
        //  do hacks 
        if(noFlashActivated) localPlayer->resetFlashDuration();
        if (isRadarActivated) doRadar();
        if (GetAsyncKeyState(VK_SPACE) & 0x8000 && isBhopActivated) doBhop();
        if (isGlowActivated) doGlow();

        //  activate/deactivate hacks
        if (GetAsyncKeyState(VK_F1) & 1) {
            noFlashActivated = !noFlashActivated;
        }
        if (GetAsyncKeyState(VK_F2) & 1) {
            isRadarActivated = !isRadarActivated;
        }
        if (GetAsyncKeyState(VK_F3) & 1) {
            isBhopActivated = !isBhopActivated;
        }
        if (GetAsyncKeyState(VK_F4) & 1) {
            isGlowActivated = !isGlowActivated;
        }
        if (GetAsyncKeyState(VK_F5) & 1) {

        }
        Sleep(1);
    }

#ifdef _DEBUG 
    if (f != nullptr) fclose(f);
    FreeConsole();
#endif 
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
