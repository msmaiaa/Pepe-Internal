#include "features.h"
#include "interfaces.h"
#include "entity.h"

#define FL_JUMP 6

LocalPlayer* localPlayer;
namespace features {
	uintptr_t clientModule;
	uintptr_t engineModule;
	uintptr_t* glowObject;
	uintptr_t* clientState;
	int input;

	bool noFlashActivated,
		isRadarActivated,
		isBhopActivated,
		isGlowActivated,
		isTbotActivated,
		isRCSActivated;
	int* localPlayerIndex;
}
void features::doRadar() {
	for (short int i = 1; i < 64; i++) {
		Ent* ent = (Ent*)interfaces::ClientEntityList->GetClientEntity(i);
		if (ent != NULL && localPlayer != NULL) {
			if (!ent->isDormant && (ent->teamNum != localPlayer->teamNum)) {
				ent->isSpotted = true;
			}
		}		
	}
};

void features::doBhop() {
	if (localPlayer != NULL) {
		if (localPlayer->velocity.isMoving() && localPlayer->flags & FL_ONGROUND) {
			*(uintptr_t*)(clientModule + offsets::dwForceJump) = 5;
		}
		else {
			*(uintptr_t*)(clientModule + offsets::dwForceJump) = 4;
		}
	}
}

void features::doGlow() {
	for (short int i = 0; i < 64; i++) {
		Ent* ent = (Ent*)interfaces::ClientEntityList->GetClientEntity(i);
		if (ent != NULL && localPlayer != NULL) {
			GlowStruct* TGlow = (GlowStruct*)(*glowObject + (ent->glowIndex * 0x38));		
			TGlow->alpha = 0.8f;
			TGlow->fullBloom = false;
			if (ent->teamNum == localPlayer->teamNum) {
				TGlow->red = 0.0f;
				TGlow->green = 0.0f;
				TGlow->blue = 1.0f;
			}
			else {
				TGlow->red = 1.0f;
				TGlow->green = 0.0f;
				TGlow->blue = 0.0f;
			}
			TGlow->renderWhenOccluded = true;
			TGlow->renderWhenNonOccluded = false;
		}
	}
}

void features::doTbot() {
	if (localPlayer != NULL) {
		if (localPlayer->crosshairId > 0 && localPlayer->crosshairId <= 64) {
			int* forceAttack = (int*)(clientModule + offsets::dwForceAttack);
			Ent* ent = (Ent*)interfaces::ClientEntityList->GetClientEntity(localPlayer->crosshairId);
			if (ent != NULL) {
				*forceAttack = 5;
				Sleep(5);
				*forceAttack = 4;
			}
		}
	}
}

void features::doRCS() {
	static uintptr_t* clientState = (uintptr_t*)(engineModule + offsets::dwClientState);
	static vec3* viewAngles = (vec3*)(*clientState + offsets::dwClientState_ViewAngles);
	static vec3 oPunch{ 0, 0, 0 };

	if (localPlayer != NULL) {
		vec3 punchAngle = localPlayer->aimPunchAngle * 2.0f;
		if (localPlayer->shotsFired > 1) {
			vec3 newAngle = *viewAngles + oPunch - punchAngle;
			newAngle.normalize();
			*viewAngles = newAngle;
		}
		oPunch = punchAngle;
	}
}

void features::setupModules() {
	engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
	clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
	clientState = (uintptr_t*)(engineModule + offsets::dwClientState);
	glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);
};