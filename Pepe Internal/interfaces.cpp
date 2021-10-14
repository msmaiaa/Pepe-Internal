#include "interfaces.h"
#include "csgosdk.h"


extern EventListener* g_eventListener;
namespace interfaces {
	IClientEntityList* ClientEntityList;
	IVEngineClient* EngineClient;
	CInput* Input;
	ISurface* Surface;
	IGameEventManager2* GameEventManager;
	IEngineTrace* EngineTrace;
}

void interfaces::setupInterfaces() {
	ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
	EngineClient = (IVEngineClient*)GetInterface(L"engine.dll", "VEngineClient014");
	GameEventManager = (IGameEventManager2*)GetInterface(L"engine.dll", "GAMEEVENTSMANAGER002");
	Surface = (ISurface*)GetInterface(L"vguimatsurface.dll", "VGUI_Surface031");
	EngineTrace = (IEngineTrace*)GetInterface(L"engine.dll", "EngineTraceClient004");
	Input = *(CInput**)(FindPattern("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);
	g_eventListener = new EventListener();
}