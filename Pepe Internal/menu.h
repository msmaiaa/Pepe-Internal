#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

namespace menu {
    extern bool opened;
    void drawMenu() noexcept;
    void drawFrame() noexcept;
    void mainLoop();
}
