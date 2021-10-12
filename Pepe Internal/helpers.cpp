#include "helpers.h"
#include "dx.h"
#include "drawing.h"
#include "entity.h"
#include "menu.h"
#include "features.h"
#include "interfaces.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx9.h"

extern LocalPlayer* localPlayer;
using namespace colors::rgb;

bool isImGuiInitialized{ false };
TrampHook* graphicsHook{ nullptr };
TrampHook* endHook{ nullptr };

typedef HRESULT(APIENTRY* tEndScene9)(LPDIRECT3DDEVICE9 pDevice);
HRESULT APIENTRY hEndScene(LPDIRECT3DDEVICE9 pdevice);

extern  LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WNDPROC oWndProc;

HWND window;
BOOL CALLBACK enumWind(HWND enumWnd, LPARAM lp) {
	DWORD procId;
	GetWindowThreadProcessId(enumWnd, &procId);
	if (GetCurrentProcessId() != procId) return TRUE;
	if (enumWnd == GetConsoleWindow()) return TRUE;

	window = enumWnd;
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (menu::opened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HWND GetProcessWindow()noexcept {
	window = NULL;
	EnumWindows(enumWind, NULL);
	return window;
}


void render::startFrame()noexcept {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void render::endFrame()noexcept {
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}


bool hook::init()noexcept {
	void* d3d9Device[119];
	if (dx::GetD3D9Device(d3d9Device)) {
		graphicsHook = new TrampHook(d3d9Device[42], hEndScene, 7);
		return true;
	}
}

void hook::destroy() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	delete graphicsHook;
	delete endHook;
	if (oWndProc)
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	oWndProc = 0;
	ImGui::DestroyContext();
}

HRESULT APIENTRY hEndScene(LPDIRECT3DDEVICE9 pdevice) {

	if (!isImGuiInitialized) {
		isImGuiInitialized = true;
		setupImGui();
		window = FindWindowA("Valve001", nullptr);
		
		oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pdevice);
	}

	render::startFrame();
	menu::drawFrame();
	render::endFrame();
	HRESULT retValue{ ((tEndScene9)graphicsHook->getGateway())(pdevice) };

	return retValue;
}

void setupImGui() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
}



