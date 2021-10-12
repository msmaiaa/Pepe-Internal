#include "features.h"
#include "interfaces.h"
#include "entity.h"
#include "EventListener.h"
#include "drawing.h"
#include <iostream>

#define FL_JUMP 6

LocalPlayer* localPlayer;
namespace features {
	uintptr_t clientModule;
	uintptr_t engineModule;
	uintptr_t* glowObject;
	uintptr_t* clientState;
	float viewMatrix[16];
	//int input;

	bool noFlashActivated,
		isRadarActivated,
		isBhopActivated,
		isGlowActivated,
		isTbotActivated,
		isRCSActivated,
		isESPActivated;
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

bool checkValidEnt(Ent* ent) {
	if (ent == NULL) return false;
	if (ent == localPlayer) return false;
	if (ent->m_iHealth <= 0) return false;
	if (ent->isDormant) return false;
	return true;
}

bool features::WorldToScreen(vec3 pos, vec2& screen) {
	vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;	
	ImVec2 ds = ImGui::GetIO().DisplaySize;
	float windowWidth = ds.x;
	float windowHeight = ds.y;
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

vec3 GetBonePos(Ent* ent, int bone) {
	uintptr_t bonePtr = ent->boneMatrix;
	vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}

vec3 TransformVec(vec3 src, vec3 ang, float d) {
	vec3 newPos;
	newPos.x = src.x + (cosf(TORAD(ang.y)) * d);
	newPos.y = src.y + (sinf(TORAD(ang.y)) * d);
	newPos.z = src.z + (tanf(TORAD(ang.x)) * d);
	return newPos;
}

void features::doESP() {
	if (localPlayer != NULL) {
		ImVec2 screenSize{ ImGui::GetIO().DisplaySize };
		for (int i = 1; i < 64; i++) {
			Ent* curEnt = (Ent*)interfaces::ClientEntityList->GetClientEntity(i);
			if (!(checkValidEnt(curEnt))) continue;

			vec3 entHead3D = GetBonePos(curEnt, 8);
			entHead3D.z += 8;
			vec2 entPos2D, entHead2D;

			if (WorldToScreen(curEnt->vecOrigin, entPos2D)) {
				//velocity esp?
				//snaplines?
				if (WorldToScreen(entHead3D, entHead2D)) {
					vec3 playerPos;
					playerPos.x = localPlayer->origin.x;
					playerPos.y = localPlayer->origin.y;
					playerPos.z = localPlayer->origin.z;
					vec3 tmpWorldPos;
					tmpWorldPos.x = curEnt->origin.x;
					tmpWorldPos.y = curEnt->origin.y;
					tmpWorldPos.z = curEnt->origin.z;
					float distance = playerPos.distance(tmpWorldPos);
					if (distance > 5.0f) {
						drawing::draw::espBox(ImVec2{ entPos2D.x , entPos2D.y }, 50.0f, distance, colors::rgb::red, curEnt->m_iHealth);
					}
				}
			}
		}
	}
}

void features::setupModules() {
	engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
	clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
	clientState = (uintptr_t*)(engineModule + offsets::dwClientState);
	glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);
};