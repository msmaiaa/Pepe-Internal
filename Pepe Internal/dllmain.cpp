#include <iostream>
#include "csgo.h"
#include "CInput.h"
#include "helpers.h"
#include "features.h"
#include "csgosdk.h"
#include "ISurface.h"
#include "entity.h"

uintptr_t DETACH_KEY = VK_END;
uintptr_t BHOP_KEY = VK_SPACE;
extern HWND window;


bool noFlashActivated, 
isRadarActivated, 
isBhopActivated,
isGlowActivated,
isTbotActivated,
isRCSActivated,
isMenuOpened = false;

LocalPlayer* localPlayer;
IClientEntityList* ClientEntityList;
CInput* Input;
ISurface* Surface;
uintptr_t clientModule;
uintptr_t engineModule;
uintptr_t* glowObject;
uintptr_t* clientState;
int* localPlayerIndex;


inline void drawMenu() noexcept {
    ImGui::Begin("Pepe", nullptr);
    ImGui::Checkbox("Glowhack", &isGlowActivated);
    ImGui::Checkbox("Radar hack", &isRadarActivated);
    ImGui::Checkbox("Bhop", &isBhopActivated);
    ImGui::Checkbox("Triggerbot", &isTbotActivated);
    ImGui::Checkbox("RCS", &isRCSActivated);
    ImGui::Checkbox("No flash", &noFlashActivated);
    ImGui::End();
}

void hook::drawFrame() noexcept {
    render::fullscreen::start();
    ImGui::GetIO().MouseDrawCursor = isMenuOpened;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
    if (isMenuOpened) {
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_None;
        drawMenu();
    }
    ImVec2 screenSize{ ImGui::GetIO().DisplaySize };;
    render::fullscreen::draw::rect(ImVec2{ 0.0f, 0.0f }, screenSize, 2.0f, colors::rgb::orange);
    render::fullscreen::end();
}

DWORD WINAPI InternalMain(HMODULE hMod) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
    ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
    Surface = (ISurface*)GetInterface(L"vguimatsurface.dll", "VGUI_Surface031");
    Input = *(CInput**)(FindPattern("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);
    engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
    clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
    clientState = (uintptr_t*)(engineModule + offsets::dwClientState);


    glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);

    if (engineModule) {
        if (hook::init()) {
            while (!GetAsyncKeyState(DETACH_KEY) & 1) {
                localPlayerIndex = (int*)(*clientState + 0x17C);
                localPlayer = (LocalPlayer*)ClientEntityList->GetClientEntity(*localPlayerIndex);
                std::cout << "localPlayer: " << std::hex << localPlayer << " - Player index: " << *localPlayerIndex << std::endl;
                //  do hacks
                if (noFlashActivated) {
                    if (localPlayer != NULL) {
                        localPlayer->resetFlashDuration();
                    }                    
                }
                if (isRadarActivated) doRadar();
                if (GetAsyncKeyState(VK_SPACE) & 0x8000 && isBhopActivated) doBhop();
                if (isGlowActivated) doGlow();
                if (isTbotActivated) doTbot();
                if (isRCSActivated) doRCS();
                
                if (GetAsyncKeyState(VK_INSERT) & 1) {
                    isMenuOpened = !isMenuOpened;
                    //InputSystem->EnableInput(!isMenuOpened);
                    if (Surface != NULL) {
                        if (isMenuOpened) {
                            //Surface->LockCursor();
                            Input->m_fMouseInitialized = false, Input->m_fMouseActive = false;
                        }
                        else {
                            Input->m_fMouseInitialized = true;
                            //Surface->UnlockCursor();
                        }
                    }
                }
                Sleep(1);
            }
            isMenuOpened = false;   
            Sleep(200);
            hook::destroy();
        }
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
