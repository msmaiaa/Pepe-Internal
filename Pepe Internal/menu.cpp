#include "menu.h"
#include <Windows.h>
#include "features.h"
#include "drawing.h"
#include "interfaces.h"
#include "entity.h"

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
    ImGui::Begin("Pepe", nullptr);
    ImGui::Checkbox("Glowhack", &features::isGlowActivated);
    ImGui::Checkbox("ESP", &features::isESPActivated);
    ImGui::Checkbox("Radar hack", &features::isRadarActivated);
    ImGui::Checkbox("Bhop", &features::isBhopActivated);
    ImGui::Checkbox("Triggerbot", &features::isTbotActivated);
    ImGui::Checkbox("RCS", &features::isRCSActivated);
    ImGui::Checkbox("No flash", &features::noFlashActivated);
   // ImGui::InputInt("text", &features::input);
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
    ImColor buff = isInGame ? green : red;
    tlMenu.add(isInGame ? "In game" : "Not in game", buff);
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