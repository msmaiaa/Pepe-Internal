#pragma once
//#include "main.h"
#include <Windows.h>
#include "_vector.h"
#include "CPlayerResource.h"
#include "entity.h"
#include <string>
#define ABS(x) ((x < 0) ? (-x) : (x))
#define TORAD(x) ((x) * 0.01745329252)
#define W2S(x, y) hack->WorldToScreen(x, y)

namespace features {
	extern uintptr_t clientModule;
	extern uintptr_t engineModule;
	extern uintptr_t* glowObject;
	extern uintptr_t* clientState;
	extern uintptr_t* PlayerResource;
	//
	extern uintptr_t killsOffset;
	extern uintptr_t deathsOffset;
	extern uintptr_t assistsOffset;
	extern uintptr_t pingOffset;
	//
	extern float viewMatrix[16];
	extern int* localPlayerIndex;;
	void doRadar();
	std::string calcCurrentKDA();
	int getPing();
	void doFov();
	void doBhop();
	void doGlow();
	void doTbot();
	void doRCS();
	void doRCSCrosshair();
	void doESP();
	void aimAt(vec3 target);
	Ent* getBestEnemy();
	void doAimbot();
	//
	void setupModules();
	void reset();
	bool WorldToScreen(vec3 pos, vec2& screen);
}