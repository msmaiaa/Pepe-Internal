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
    ImGui::GetWindowDrawList()->AddLine(from, to, black, width + 2);
    ImGui::GetWindowDrawList()->AddLine(from, to, color, width);
}

void drawing::draw::text(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center) noexcept {

    if (center) {
        const auto txtSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
        ImVec2 center{ pos.x - (txtSize.x / 2),pos.y - (txtSize.y / 2) };
        //ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size + 1, pos, black, text.c_str());
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos, color, text.c_str());
        return;
    }
    //ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size + 2, pos, black, text.c_str());
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

void drawing::draw::espBox(const ImVec2& pos, float characterHeight, float distance, const ImColor& color, const int healthPercent) noexcept {
    float lineWidth = 1.0f;

    ImVec2 display{ ImGui::GetIO().DisplaySize };
    float height = (display.y / distance) * characterHeight;
    float width = (display.x / distance) * characterHeight / 3;

    //SnapLines:
    //drawing::draw::line(ImVec2{ display.x / 2.0f, display.y }, pos, lineWidth / 2, color);

    //Box:
    drawing::draw::rect(ImVec2{ pos.x - width / 2, pos.y - height }, ImVec2{ pos.x + width / 2, pos.y }, lineWidth, color);

    /*if (healthPercent != -1) {
        float perc = (width / 100);
        float curr = perc * healthPercent;

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2{ pos.x - (width / 2) - 1, (pos.y - height / characterHeight - 1) - height * 1.2f }, ImVec2{ pos.x + width / 2 + 1, (pos.y + height / characterHeight + 1) - height * 1.2f }, black);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2{ pos.x - (width / 2), (pos.y - height / characterHeight) - height * 1.2f }, ImVec2{ pos.x + width / 2, (pos.y + height / characterHeight) - height * 1.2f }, lightgray);

        ImColor hColor{ 1.0f - (healthPercent / 100.0f), healthPercent / 100.0f, 0.0f, 0.7f };
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2{ pos.x - (width / 2), (pos.y - height / characterHeight) - height * 1.2f }, ImVec2{ pos.x - (width / 2) + curr, (pos.y + height / characterHeight) - height * 1.2f }, hColor);
    }*/
}