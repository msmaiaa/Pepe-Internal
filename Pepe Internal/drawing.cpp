#include "drawing.h"
using namespace colors::rgb;
void drawing::start() noexcept {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGui::Begin("##Canvas", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMouseInputs);
}

void drawing::end() noexcept {
    ImGui::GetWindowDrawList()->PushClipRectFullScreen();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void drawing::draw::line(const ImVec2& from, const ImVec2& to, float width, const ImColor& color) noexcept {
    //ImGui::GetWindowDrawList()->AddLine(from, to, black, width + 2);
    ImGui::GetWindowDrawList()->AddLine(from, to, color, width);
}

void drawing::draw::text(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center) noexcept {
    if (center) {
        const auto txtSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
        ImVec2 center{ pos.x - (txtSize.x / 2),pos.y - (txtSize.y / 2) };
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos, color, text.c_str());
        return;
    }
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos, color, text.c_str());
}

void drawing::draw::rect(const ImVec2& upperLeft, const ImVec2& lowerRight, float width, const ImColor& color) noexcept {
    ImGui::GetWindowDrawList()->AddRect(upperLeft, lowerRight, black, 0.0f, 15, width + 2);
    ImGui::GetWindowDrawList()->AddRect(upperLeft, lowerRight, color, 0.0f, 15, width);
}

void drawing::draw::circle(const ImVec2& pos, float radius, const ImColor& color) noexcept {
    ImGui::GetWindowDrawList()->AddCircle(pos, radius, black, 0, 3.0f);
    ImGui::GetWindowDrawList()->AddCircle(pos, radius, color);
}

void drawing::draw::espBox(const ImVec2& top, const ImVec2 bot, int thickness, const ImColor& color, const int healthPercent) noexcept {
    int height = (top.y - bot.y);
    ImVec2 display{ ImGui::GetIO().DisplaySize };
    ImVec2 tl, br;
    tl.x = top.x - height / 4;
    tl.y = top.y;
    br.x = bot.x + height / 4;
    br.y = bot.y;

    drawing::draw::rect(tl, br, thickness, color);

}