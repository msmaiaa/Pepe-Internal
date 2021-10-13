#pragma once
//#include "main.h"
#include <Windows.h>
#include "_vector.h"
#include "CPlayerResource.h"
#define ABS(x) ((x < 0) ? (-x) : (x))
#define TORAD(x) ((x) * 0.01745329252)
#define W2S(x, y) hack->WorldToScreen(x, y)

namespace features {
	extern uintptr_t clientModule;
	extern uintptr_t engineModule;
	extern uintptr_t* glowObject;
	extern uintptr_t* clientState;
	extern C_PlayerResource* PlayerResource;
	extern float viewMatrix[16];
	extern int* localPlayerIndex;;
	void doRadar();
	void doBhop();
	void doGlow();
	void doTbot();
	void doRCS();
	void doRCSCrosshair();
	void doESP();
	void setupModules();
	bool WorldToScreen(vec3 pos, vec2& screen);
}