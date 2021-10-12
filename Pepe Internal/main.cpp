#include "main.h"
#include "helpers.h"
#include "features.h"
#include "interfaces.h"
#include "menu.h"

uintptr_t DETACH_KEY = VK_END;
extern EventListener* g_eventListener;
DWORD WINAPI InternalMain(HMODULE hMod) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
    if (hook::init()) {
        features::setupModules();
        interfaces::setupInterfaces();
        while (!GetAsyncKeyState(DETACH_KEY) & 1) {
            menu::mainLoop();
        }
        menu::opened = false;   
        if (g_eventListener != NULL) {
            g_eventListener->Remove();
            delete g_eventListener;
        }
        Sleep(500);
        hook::destroy();
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
