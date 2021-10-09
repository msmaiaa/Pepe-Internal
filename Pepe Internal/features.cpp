#include "entity.h"
#include "csgo.h"
#include <iostream>
#define FL_ONGROUND (1 << 0)
#define FL_JUMP 6

extern IClientEntityList* ClientEntityList;
extern LocalPlayer* localPlayer;
extern uintptr_t clientModule;
extern uintptr_t* glowObject;
static bool lastjump = false;

void doRadar() {
	for (short int i = 1; i < 64; i++) {
		Ent* ent = (Ent*)ClientEntityList->GetClientEntity(i);
		if (ent != NULL) {
			if (!ent->isDormant && (ent->teamNum != localPlayer->teamNum)) {
				ent->isSpotted = true;
			}
		}		
	}
};

void doBhop() {
	if (localPlayer->velocity.isMoving() && localPlayer->flags & FL_ONGROUND) {
		*(uintptr_t*)(clientModule + offsets::dwForceJump) = 5;
		lastjump = true;
	}
	else {
		*(uintptr_t*)(clientModule + offsets::dwForceJump) = 4;
	}
}

void doGlow() {
	for (short int i = 0; i < 64; i++) {
		Ent* ent = (Ent*)ClientEntityList->GetClientEntity(i);
		if (ent != NULL) {
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