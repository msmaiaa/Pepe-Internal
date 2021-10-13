#include "menu.h"
#include <Windows.h>
#include "features.h"
#include "drawing.h"
#include "interfaces.h"
#include "entity.h"
#include <iostream>

using namespace colors::rgb;
extern LocalPlayer* localPlayer;
uintptr_t BHOP_KEY = VK_SPACE;
namespace menu {
    bool opened;
}
std::string hexToStr(int n) {
    std::stringstream ss;
    ss << std::hex << n;
    return ss.str();
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
            drawing::draw::text(i.text, i.pos, 16.0f, i.color);
        }
    }
};

void menu::drawMenu() noexcept {
    static int tabb = 0;
    static ImVec4 buttonActive = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
    static ImVec4 buttonDefault = ImVec4(0.490f, 0.490f, 0.490f, 1.00f);
    ImGuiStyle* style = &ImGui::GetStyle();
    ImGui::Begin("Pepe", nullptr);
    const char* tabs[] = {
        "Visuals",
        "Triggerbot",
        "Misc",
        "Config"
    };
    for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
    {
        int distance = i == tabb ? 0 : i > tabb ? i - tabb : tabb - i;

        ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
            buttonActive.x - (distance * 0.055f),
            buttonActive.y - (distance * 0.055f),
            buttonActive.z - (distance * 0.055f),
            buttonActive.w
        );

        if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
            tabb = i;

        ImGui::GetStyle().Colors[ImGuiCol_Button] = buttonDefault;

        if (i < IM_ARRAYSIZE(tabs) - 1)
            ImGui::SameLine();
    }

    ImGui::Separator();

    switch (tabb) {
    case 0: 
        ImGui::Checkbox("ESP", &features::isESPActivated);
        ImGui::Checkbox("Radar", &features::isRadarActivated);
        ImGui::Checkbox("Glowhack", &features::isGlowActivated);        
        break;
    
    case 1: 
        ImGui::Checkbox("Toggled", &features::isTbotActivated);
        ImGui::PushItemWidth(22.0f);
        ImGui::InputInt("Delay", &features::tBotDelay, 0);
        ImGui::PopItemWidth();
        break;
   
    case 2: 
        ImGui::Checkbox("Bhop", &features::isBhopActivated);
        ImGui::Checkbox("RCS", &features::isRCSActivated);
        ImGui::Checkbox("No flash", &features::noFlashActivated);
        break;
    case 3:
    {
        ImGui::Button("Load config", ImVec2(100, 25));
        ImGui::Button("Save config", ImVec2(100, 25));
    }
        break;
    }
    ImGui::End();
}


void menu::drawFrame() noexcept {
    ImGui::GetIO().MouseDrawCursor = opened;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange;
    if (opened) {
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_None;
        drawMenu();
    }
    drawing::start();
    if (features::isESPActivated) features::doESP();
    bool isInGame = interfaces::EngineClient->IsInGame();
    ItemDrawer tlMenu;
    tlMenu.pos = "topleft";
    player_info_s playerInfo;
    if (interfaces::EngineClient->GetPlayerInfo(*features::localPlayerIndex, &playerInfo)) {
        std::string n(playerInfo.szName);
        std::string s(playerInfo.szSteamID);
        tlMenu.add(n + ":" + s, green);
    }
    ImColor buff = isInGame ? green : red;
    tlMenu.add(localPlayer != NULL ? "LocalPlayer: 0x" + hexToStr((int)localPlayer) : "LocalPlayer not found", localPlayer != NULL ? green : red);
    tlMenu.send();

    drawing::end();
}

void menu::mainLoop() {
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        opened = !opened;
        if (interfaces::Surface != NULL) {
            if (opened) {
                interfaces::Input->m_fMouseInitialized = false, interfaces::Input->m_fMouseActive = false;
            }
            else {
                interfaces::Input->m_fMouseInitialized = true;
            }
        }
    }
    if (interfaces::EngineClient->IsInGame()) {
        memcpy(&features::viewMatrix, (PBYTE*)(features::clientModule + offsets::dwViewMatrix), sizeof(features::viewMatrix));
        features::localPlayerIndex = (int*)(*features::clientState + 0x17C);
        localPlayer = (LocalPlayer*)interfaces::ClientEntityList->GetClientEntity(*features::localPlayerIndex);
        if (features::noFlashActivated) {
            if (localPlayer != NULL) {
                localPlayer->resetFlashDuration();
            }
        }
        if (features::isRadarActivated) features::doRadar();
        if (GetAsyncKeyState(VK_SPACE) & 0x8000 && features::isBhopActivated) features::doBhop();
        if (features::isGlowActivated) features::doGlow();
        if (features::isTbotActivated) features::doTbot();
        if (features::isRCSActivated) features::doRCS();
    }
    Sleep(1);
}