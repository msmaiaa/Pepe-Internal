#pragma once
//#include "main.h"
#include <Windows.h>

namespace features {
	extern uintptr_t clientModule;
	extern uintptr_t engineModule;
	extern uintptr_t* glowObject;
	extern uintptr_t* clientState;
	extern int input;

	extern bool noFlashActivated,
		isRadarActivated,
		isBhopActivated,
		isGlowActivated,
		isTbotActivated,
		isRCSActivated;
	extern int* localPlayerIndex;
	void doRadar();
	void doBhop();
	void doGlow();
	void doTbot();
	void doRCS();
	void setupModules();
}