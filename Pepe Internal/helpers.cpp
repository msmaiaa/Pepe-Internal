#include "helpers.h"
#include "dx.h"
#include "drawing.h"
#include "entity.h"
#include "menu.h"
#include "features.h"
#include "interfaces.h"
#include "imgui_h.h"

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
	ImGuiStyle* style = &ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();
	style->FramePadding = ImVec2(0, 0);
	style->WindowMinSize = ImVec2(700, 20);
	style->WindowPadding = ImVec2(15, 10);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 0.0f;
	style->ItemSpacing = ImVec2(4, 4);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 14.0f;
	style->GrabRounding = 0.0f;

	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.043f, 0.054f, 0.058f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.137f, 0.141f, 0.145f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.349f, 0.349f, 0.349f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.231f, 0.247f, 0.250f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
}



