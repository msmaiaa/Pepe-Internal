#include "interfaces.h"
#include "csgosdk.h"

namespace interfaces {
	IClientEntityList* ClientEntityList;
	IVEngineClient* EngineClient;
	CInput* Input;
	//C_PlayerResource* PlayerResource;
	ISurface* Surface;
}

void interfaces::setupInterfaces() {
	ClientEntityList = (IClientEntityList*)GetInterface(L"client.dll", "VClientEntityList003");
	EngineClient = (IVEngineClient*)GetInterface(L"engine.dll", "VEngineClient014");
	Surface = (ISurface*)GetInterface(L"vguimatsurface.dll", "VGUI_Surface031");
	Input = *(CInput**)(FindPattern("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);
}