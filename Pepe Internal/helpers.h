#pragma once
#include <string>
#include <Windows.h>

HWND GetProcessWindow() noexcept;
enum class KeyState
{
	None = 1,
	Down,
	Up,
	Pressed /*Down and then up*/
};

void setupImGui();
namespace render {
	void startFrame() noexcept;
	void endFrame() noexcept;
	void updateOverlayState(bool state, HWND ownd) noexcept;
}

namespace Time {
	float deltaTimeSec();
}

namespace hook {
	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
	bool init() noexcept;
	void destroy() noexcept;
	void drawFrame() noexcept;
}


