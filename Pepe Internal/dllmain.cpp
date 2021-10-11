#include <iostream>
#include "csgo.h"
#include "CInput.h"
#include "helpers.h"
#include "features.h"
#include "csgosdk.h"
#include "CPlayerResource.h"
#include "ISurface.h"
#include "IVEngineClient.h"
#include "entity.h"
#include <sstream>

uintptr_t DETACH_KEY = VK_END;
uintptr_t BHOP_KEY = VK_SPACE;
extern HWND window;

using namespace colors::rgb;

bool noFlashActivated, 
isRadarActivated, 
isBhopActivated,
isGlowActivated,
isTbotActivated,
isRCSActivated,
isMenuOpened = false;

LocalPlayer* localPlayer;
IClientEntityList* ClientEntityList;
IVEngineClient* EngineClient;
CInput* Input;
//C_PlayerResource* PlayerResource;
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


struct TextDrawParams {
    std::string text;
    ImColor color;
    ImVec2 pos = {};
    //bool center;
};

struct ItemDrawer {
    std::vector<TextDrawParams> items;
    std::string pos;
    int nextY = 5;
    int nextX = 0;
    ImVec2 screenSize{ ImGui::GetIO().DisplaySize };
    void add(std::string text, ImColor color) {
        TextDrawParams newItem{
            text,
            color
        };
        newItem.text = text;
        newItem.color = color;
        if (pos.compare("topleft") == 0) {
            if (nextX == 0) {
                nextX = (screenSize.x - screenSize.x) + 350;
            }
            newItem.pos.x = nextX;
            newItem.pos.y = nextY;
            nextY += 20;
            items.push_back(newItem);
        }
        if (pos.compare("topright") == 0) {

        }
        if (pos.compare("bottomleft") == 0) {

        }
        if (pos.compare("bottomright") == 0) {

        }
    }
    void send() {
        for (TextDrawParams i : items) {
            render::fullscreen::draw::text(i.text, i.pos, 16.0f, i.color);
        }
    }
};

std::string hexToStr(int n) {
    std::stringstream ss;
    ss << std::hex << n;
    return ss.str();
}

void hook::drawFrame() noexcept {
    ImGui::GetIO().MouseDrawCursor = isMenuOpened;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
    if (isMenuOpened) {
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_None;
        drawMenu();
    }
    render::fullscreen::start();

    bool isInGame = EngineClient->IsInGame();
    //render::fullscreen::draw::text(isInGame ? "In game" : "Not in game", textLocation, 16.0f, isInGame ? colors::rgb::green : colors::rgb::red);
    ItemDrawer tlMenu;
    tlMenu.pos = "topleft";
    ImColor buff = isInGame ? green : red;
    tlMenu.add(isInGame ? "In game" : "Not in game", buff);
    tlMenu.add(localPlayer != NULL ? "LocalPlayer: 0x" + hexToStr((int)localPlayer) : "LocalPlayer not found", localPlayer != NULL ? green : red);
    tlMenu.send();

    //render::fullscreen::draw::rect(ImVec2{ 0.0f, 0.0f }, screenSize, 2.0f, colors::rgb::orange);
    render::fullscreen::end();
}

DWORD WINAPI InternalMain(HMODULE hMod) {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
    ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
    EngineClient = (IVEngineClient*)GetInterface(L"engine.dll", "VEngineClient014");
    Surface = (ISurface*)GetInterface(L"vguimatsurface.dll", "VGUI_Surface031");
    Input = *(CInput**)(FindPattern("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);
    engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
    clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
    //PlayerResource = (C_PlayerResource*)(clientModule + offsets::dwPlayerResource);
    clientState = (uintptr_t*)(engineModule + offsets::dwClientState);


    glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);

    if (engineModule) {
        if (hook::init()) {
            while (!GetAsyncKeyState(DETACH_KEY) & 1) {
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
                if (EngineClient->IsInGame()) {
                    localPlayerIndex = (int*)(*clientState + 0x17C);
                    localPlayer = (LocalPlayer*)ClientEntityList->GetClientEntity(*localPlayerIndex);          
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
