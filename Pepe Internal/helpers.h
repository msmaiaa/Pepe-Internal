#pragma once
#include <string>

#include "internal.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3d9.h>
#include <d3dx9.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

enum class KeyState
{
	None = 1,
	Down,
	Up,
	Pressed /*Down and then up*/
};

HWND GetProcessWindow() noexcept;

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

namespace render {
	void startFrame() noexcept;
	void endFrame() noexcept;
	void updateOverlayState(bool state, HWND ownd) noexcept;
	namespace fullscreen {
		void start() noexcept;
		void end() noexcept;

		namespace draw {
			void line(const ImVec2& from, const ImVec2& to, float width, const ImColor& color) noexcept;
			void text(const std::string& text, const ImVec2& pos, float size, float width, const ImColor& color, bool center = true) noexcept;
			void rect(const ImVec2& upperLeft, const ImVec2& lowerRight, float width, const ImColor& color) noexcept;
			void circle(const ImVec2& pos, float radius, const ImColor& color) noexcept;
		}
	}
}

namespace hook {
	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	bool init() noexcept;
	void destroy() noexcept;
	void drawFrame() noexcept;
}