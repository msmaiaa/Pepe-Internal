#include "entity.h"
#include "features.h"
#include "csgo.h"
#include <iostream>
#define FL_ONGROUND (1 << 0)
#define FL_JUMP 6

extern IClientEntityList* ClientEntityList;
extern LocalPlayer* localPlayer;
extern uintptr_t clientModule;
extern uintptr_t engineModule;
extern uintptr_t* glowObject;
Colors colors;
static bool lastjump = false;

void doRadar() {
	for (short int i = 1; i < 64; i++) {
		Ent* ent = (Ent*)ClientEntityList->GetClientEntity(i);
		if (ent != NULL && localPlayer != NULL) {
			if (!ent->isDormant && (ent->teamNum != localPlayer->teamNum)) {
				ent->isSpotted = true;
			}
		}		
	}
};

void doBhop() {
	if (localPlayer != NULL) {
		if (localPlayer->velocity.isMoving() && localPlayer->flags & FL_ONGROUND) {
			*(uintptr_t*)(clientModule + offsets::dwForceJump) = 5;
			lastjump = true;
		}
		else {
			*(uintptr_t*)(clientModule + offsets::dwForceJump) = 4;
		}
	}
}

void doGlow() {
	for (short int i = 0; i < 64; i++) {
		Ent* ent = (Ent*)ClientEntityList->GetClientEntity(i);
		if (ent != NULL && localPlayer != NULL) {
			GlowStruct* TGlow = (GlowStruct*)(*glowObject + (ent->glowIndex * 0x38));		
			TGlow->alpha = 0.8f;
			TGlow->fullBloom = false;
			if (ent->teamNum == localPlayer->teamNum) {
				TGlow->red = colors.blue[0];
				TGlow->green = colors.blue[1];
				TGlow->blue = colors.blue[2];
			}
			else {
				TGlow->red = colors.red[0];
				TGlow->green = colors.red[1];
				TGlow->blue = colors.red[2];
			}
			TGlow->renderWhenOccluded = true;
			TGlow->renderWhenNonOccluded = false;
		}
	}
}

void doTbot() {
	if (localPlayer != NULL) {
		if (localPlayer->crosshairId > 0 && localPlayer->crosshairId <= 64) {
			int* forceAttack = (int*)(clientModule + offsets::dwForceAttack);
			Ent* ent = (Ent*)ClientEntityList->GetClientEntity(localPlayer->crosshairId);
			if (ent != NULL) {
				*forceAttack = 5;
				Sleep(5);
				*forceAttack = 4;
			}
		}
	}
}

void doRCS() {
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