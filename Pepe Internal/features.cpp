#include "entity.h"
#include "csgo.h"
#include <iostream>
#define FL_ONGROUND (1 << 0)
#define FL_JUMP 6

extern IClientEntityList* ClientEntityList;
extern LocalPlayer* localPlayer;
extern uintptr_t clientModule;
static bool lastjump = false;

void doRadar() {
	for (int i = 1; i < 64; i++) {
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