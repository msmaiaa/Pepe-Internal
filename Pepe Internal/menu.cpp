#include "menu.h"
#include <Windows.h>
#include "features.h"
#include "drawing.h"
#include "interfaces.h"
#include "entity.h"
#include "config.h"
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

void drawIntInput(const char* label, int& v, float width) {
    ImGui::PushItemWidth(width);
    ImGui::InputInt(label, &v, 0);
    ImGui::PopItemWidth();
}

void menu::drawMenu() noexcept {
    static int tabb = 0;
    static ImVec4 buttonActive = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
    static ImVec4 buttonDefault = ImVec4(0.490f, 0.490f, 0.490f, 1.00f);
    ImGuiStyle* style = &ImGui::GetStyle();
    ImGui::Begin("Pepe", nullptr);
    const char* tabs[] = {
        "Visuals",
        "Triggerbot",
        "Aimbot",
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
        //VISUALS
        ImGui::Checkbox("Glow", &config::isGlowActivated);        
        ImGui::Checkbox("Glow allies", &config::glow_allies);        
        ImGui::Checkbox("ESP", &config::isESPActivated);
        ImGui::Checkbox("Show allies on ESP", &config::esp_allies);
        ImGui::Checkbox("Radar", &config::isRadarActivated);
        ImGui::Checkbox("RCS Crosshair", &config::isRCSCrosshairActivated);
        ImGui::Checkbox("Show aimbot fov", &config::drawAimbotFov);
        break;
    
    case 1: 
        //TRIGGER
        ImGui::Checkbox("Triggerbot toggled", &config::isTbotActivated);
        drawIntInput("Delay", config::tBotDelay, 25.0f);
        break;
   
    case 2: 
        //AIMBOT
        ImGui::Checkbox("Aimbot toggled", &config::isAimbotActivated);
        drawIntInput("Aimbot fov", config::aimbotFov, 25.0f);
        ImGui::Checkbox("Smooth aimbot", &config::smoothAimbot);
        if (config::smoothAimbot) {
            drawIntInput("Aimbot speed", config::aimbotSpeed, 25.0f);
        }
        break;
    case 3:
        //MISC
        ImGui::Checkbox("Bhop", &config::isBhopActivated);
        ImGui::Checkbox("RCS", &config::isRCSActivated);
        ImGui::Checkbox("No flash", &config::noFlashActivated);
        ImGui::Checkbox("Fov changer", &config::isFovActivated);
        if (config::isFovActivated) {
            drawIntInput("Fov", config::fovAmount, 30.0f);
        }
        break;
    case 4:
        //CONFIG
        if (ImGui::Button("Load config", ImVec2(100, 25))) {
            config::loadConfig();
        };
        if (ImGui::Button("Save config", ImVec2(100, 25))) {
            config::saveConfig();
        };
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
    if (config::isESPActivated) features::doESP();
    if (config::isRCSCrosshairActivated) features::doRCSCrosshair();
    ImVec2 display{ ImGui::GetIO().DisplaySize };
    if (config::drawAimbotFov) {
        if (config::aimbotFov > 0) {
            float centerY = display.y / 2.0f;
            float centerX = display.x / 2.0f;
            //static float maxDist{ config::aimbotFov / (90 / centerY) };
            //std::cout << maxDist << std::endl;
            drawing::draw::circle(ImVec2{ centerX, centerY }, config::aimbotFov / (90 / centerY), green);
        }
    }
    ///topleft menu
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
    ///
    drawing::end();
}

int started = false;
void menu::mainLoop() {
    if (!started) {
        config::loadConfig();
        started = true;
    }
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
        if (config::noFlashActivated) {
            if (localPlayer != NULL) {
                localPlayer->resetFlashDuration();
            }
        }
        if (config::isRadarActivated) features::doRadar();
        if (config::isFovActivated) features::doFov();
        if (GetAsyncKeyState(VK_SPACE) & 0x8000 && config::isBhopActivated) features::doBhop();
        if (config::isGlowActivated) features::doGlow();
        if (config::isTbotActivated) features::doTbot();
        //if (config::isRCSActivated) features::doRCS();
        if (GetAsyncKeyState(config::aimbotKey) && config::isAimbotActivated) features::doAimbot();
    }
    Sleep(1);
}