#ifndef DRAWING_H
#define DRAWING_H
//#pragma once
#include <string>
#include "imgui_h.h"

namespace colors {
    namespace rgb {
        const ImColor red{ 1.0f , 0.0f, 0.0f, 0.7f };
        const ImColor green{ 0.0f, 1.0f, 0.0f, 0.7f };
        const ImColor blue{ 0.0f, 0.0f, 1.0f, 0.7f };
        const ImColor gray{ 0.2f, 0.2f, 0.2f, 0.7f };
        const ImColor lightgray{ 0.7f , 0.7f, 0.7f, 0.7f };
        const ImColor yellow{ 1.0f, 1.0f, 0.0f, 0.7f };
        const ImColor orange{ 0.7f, 0.5f, 0.0f, 0.7f };
        const ImColor pink{ 0.7f, 0.5f, 0.7f, 0.7f };
        const ImColor cyan{ 0.0f, 0.7f, 0.7f, 0.7f };
        const ImColor black{ 0.0f, 0.0f, 0.0f, 0.7f };
    }
}

namespace drawing {
    void start() noexcept;
    void end() noexcept;
    namespace draw {
        void line(const ImVec2& from, const ImVec2& to, float width, const ImColor& color) noexcept;
        void text(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center = true) noexcept;
        void rect(const ImVec2& upperLeft, const ImVec2& lowerRight, float width, const ImColor& color) noexcept;
        void circle(const ImVec2& pos, float radius, const ImColor& color) noexcept;
        void espBox(const ImVec2& top, const ImVec2 bot, int thickness, const ImColor& color, const int healthPercent = -1) noexcept;
    }
}

#endif DRAWING_H