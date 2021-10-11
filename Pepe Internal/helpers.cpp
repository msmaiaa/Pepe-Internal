#include "helpers.h"
#include "dx.h"
#include <string>
#include <sstream>

bool isImGuiInitialized{ false };
extern bool isMenuOpened;
HHOOK MouseHook;

using namespace colors::rgb;

TrampHook* graphicsHook{ nullptr };
TrampHook* endHook{ nullptr };

typedef HRESULT(APIENTRY* tEndScene9)(LPDIRECT3DDEVICE9 pDevice);
HRESULT APIENTRY hEndScene(LPDIRECT3DDEVICE9 pdevice);

typedef long(__stdcall* Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
//static Reset oReset = NULL;

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
	//if (uMsg == WM_KEYUP && wParam == VK_INSERT)
		//gui->toggle_active();
	if (isMenuOpened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	//return (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && isMenuOpened) ? 1 : CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
	/*if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && isMenuOpened)
	{
		return 1;
	}
	*/
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

void render::fullscreen::start() noexcept {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

	ImGui::Begin("##Canvas", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMouseInputs);
}

void render::fullscreen::end() noexcept {
	ImGui::GetWindowDrawList()->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void render::fullscreen::draw::line(const ImVec2& from, const ImVec2& to, float width, const ImColor& color) noexcept {
	ImGui::GetWindowDrawList()->AddLine(from, to, black, width + 2);
	ImGui::GetWindowDrawList()->AddLine(from, to, color, width);
}

void render::fullscreen::draw::text(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center) noexcept {

	if (center) {
		const auto txtSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
		ImVec2 center{ pos.x - (txtSize.x / 2),pos.y - (txtSize.y / 2) };
		//ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size + 2, pos, black, text.c_str());
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos, color, text.c_str());
		return;
	}
	//ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size + 2, pos, black, text.c_str());
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos, color, text.c_str());
}

void render::fullscreen::draw::rect(const ImVec2& upperLeft, const ImVec2& lowerRight, float width, const ImColor& color) noexcept {
	ImGui::GetWindowDrawList()->AddRect(upperLeft, lowerRight, black, 0.0f, 15, width + 2);
	ImGui::GetWindowDrawList()->AddRect(upperLeft, lowerRight, color, 0.0f, 15, width);
}

void render::fullscreen::draw::circle(const ImVec2& pos, float radius, const ImColor& color) noexcept {
	ImGui::GetWindowDrawList()->AddCircle(pos, radius, black, 0, 3.0f);
	ImGui::GetWindowDrawList()->AddCircle(pos, radius, color);
}

/*long __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	long result = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}*/

bool hook::init()noexcept {
	void* d3d9Device[119];
	if (dx::GetD3D9Device(d3d9Device)) {
		graphicsHook = new TrampHook(d3d9Device[42], hEndScene, 7);
		//endHook = new TrampHook(d3d9Device[16], hkReset, 7);
		return true;
	}
}

void hook::destroy() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	delete graphicsHook; //delete the hook
	delete endHook;
	if (oWndProc)
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	oWndProc = 0;
	ImGui::DestroyContext();
}

HRESULT APIENTRY hEndScene(LPDIRECT3DDEVICE9 pdevice) {

	if (!isImGuiInitialized) {
		isImGuiInitialized = true;
		//setup ImGui:
		ImGui::CreateContext();
		window = FindWindowA("Valve001", nullptr);
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		

		oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
		//ImGui_ImplWin32_Init(GetProcessWindow());
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pdevice);
	}

	//set up frame:
	render::startFrame();

	//actual drawing of the Frame:
	hook::drawFrame();

	//end of frame:
	render::endFrame();
	

	//call the gateway as if it was the original function:
	HRESULT retValue{ ((tEndScene9)graphicsHook->getGateway())(pdevice) };

	return retValue;
}
