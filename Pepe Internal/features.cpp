#include "features.h"
#include "interfaces.h"
#include "entity.h"
#include "EventListener.h"
#include "drawing.h"
#include "config.h"
#include "helpers.h"
#include "netvars.h"
#include <iostream>
#include <thread>
#include <iomanip>


#define FL_JUMP 6

LocalPlayer* localPlayer;
vec3 newAngles{ 0, 0, 0 };
namespace features {
	uintptr_t clientModule;
	uintptr_t engineModule;
	uintptr_t* glowObject;
	uintptr_t* clientState;
	uintptr_t* PlayerResource;
	//
	uintptr_t killsOffset;
	uintptr_t deathsOffset;
	uintptr_t assistsOffset;
	uintptr_t pingOffset;
	//
	float viewMatrix[16];
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
			if (ent->teamNum == localPlayer->teamNum && !config::glow_allies) continue;
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
		if (GetAsyncKeyState(config::tBotKey)) {
			if (localPlayer->crosshairId > 0 && localPlayer->crosshairId <= 64) {
				Ent* ent = (Ent*)interfaces::ClientEntityList->GetClientEntity(localPlayer->crosshairId);
				if (ent->teamNum != localPlayer->teamNum) {
					int* forceAttack = (int*)(clientModule + offsets::dwForceAttack);
					if (ent != NULL) {
						Sleep(config::tBotDelay);
						*forceAttack = 5;
						Sleep(5);
						*forceAttack = 4;
					}
				}
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
		newAngles = punchAngle;
	}
}

void features::doRCSCrosshair() {
	if (localPlayer != NULL) {
		vec3 punchAngle = localPlayer->aimPunchAngle;
		ImVec2 display{ ImGui::GetIO().DisplaySize };
		ImVec2 crosshair2D;
		crosshair2D.x = display.x / 2 - (display.x / 90 * punchAngle.y);
		crosshair2D.y = display.y / 2 + (display.y / 90 * punchAngle.x);
		ImVec2 from = crosshair2D;
		//
		drawing::draw::line(ImVec2(from.x, from.y - 4), ImVec2(from.x, from.y + 4), 1.0f, colors::rgb::red);
		drawing::draw::line(ImVec2(from.x + 4, from.y), ImVec2(from.x - 4, from.y), 1.0f, colors::rgb::red);
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
			if (curEnt->teamNum == localPlayer->teamNum && !config::esp_allies) continue;
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
						drawing::draw::espBox(ImVec2(entPos2D.x, entPos2D.y), ImVec2(entHead2D.x, entHead2D.y), 2, colors::rgb::red, curEnt->m_iHealth);
					}
				}
			}
		}
	}
}

Ent* features::getBestEnemy() {
	Ent* bestEnemy = nullptr;
	int bestEnemyIndex = -1;
	float lastDistance = 100000;
	for (int i = 1; i < 64; i++) {
		Ent* curEnt = (Ent*)interfaces::ClientEntityList->GetClientEntity(i);
		if (curEnt == NULL || curEnt == localPlayer) continue;
		if (curEnt->teamNum == localPlayer->teamNum || curEnt->isDormant) continue;
		if (curEnt->m_iHealth < 1 || localPlayer->m_iHealth < 1) continue;

		vec3 entHead3D = GetBonePos(curEnt, 8);
		vec2 entPos2D, entHead2D;
		if (WorldToScreen(entHead3D, entPos2D)) {
			ImVec2 displaySize = ImGui::GetIO().DisplaySize;
			float centerX = displaySize.x / 2.0f;
			float centerY = displaySize.y / 2.0f;
			float distY{ centerY - entPos2D.y  };
			float distX{ centerX - entPos2D.x };
			float tmpDistance = sqrtf((distX * distX) + (distY * distY));
			float maxDist = config::aimbotFov / (90 / centerY);
			if (tmpDistance > maxDist) continue;
			if (!lastDistance) {
				lastDistance = tmpDistance;
				continue;
			}
			if (tmpDistance < lastDistance) {
				lastDistance = tmpDistance;
				bestEnemy = curEnt;
			}
			else continue;
		}
	}
	return bestEnemy;
}
double PI = 3.14159265358;
void features::aimAt(vec3 target) {
	static vec3* viewAngles = (vec3*)(*clientState + offsets::dwClientState_ViewAngles);

	vec3 myCamPos{ localPlayer->origin + localPlayer->m_vecViewOffset };

	vec3 origin = localPlayer->vecOrigin;
	vec3 viewOffset = localPlayer->m_vecViewOffset;
	vec3 myPos = (origin + viewOffset);

	vec3 deltaVec = { target.x - myPos.x, target.y - myPos.y, target.z - myPos.z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);



	float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
	float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

	//calc angle diff
	vec3 punchAngle = localPlayer->aimPunchAngle;

	if (config::smoothAimbot) {
		float diffX{ fmodf(pitch - viewAngles->x, 180.0f) - (punchAngle.x * 2.0f) };
		float diffY{ fmodf(yaw - viewAngles->y, 180.0f) - (punchAngle.y * 2.0f) };
		diffX = fmodf(2 * diffX, 180.0f) - diffX;
		diffY = fmodf(2 * diffY, 180.0f) - diffY;
		float aimspeed{ (float)config::aimbotSpeed };
		aimspeed *= Time::deltaTimeSec();
		float precision{ 0.02f };
		precision *= precision;
		if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
		{

			if (diffX * diffX > precision) {
				viewAngles->x += (diffX * aimspeed);
			}
			if (diffY * diffY > precision) {
				viewAngles->y += (diffY * aimspeed);
			}
		}
	}
	else {
		viewAngles->x = pitch - (punchAngle.x * 2.0f);
		viewAngles->y = yaw - (punchAngle.y * 2.0f);
	}
}


Ent* GetClosestEnemy() {
	float closestDistance = 10000000;
	int closestDistanceIndex = -1;

	for (int i = 1; i < 64; i++) {
		Ent* curEnt = (Ent*)interfaces::ClientEntityList->GetClientEntity(i);
		if (curEnt == NULL || curEnt == localPlayer) continue;
		if (curEnt->teamNum == localPlayer->teamNum || curEnt->isDormant) continue;
		if (curEnt->m_iHealth < 1 || localPlayer->m_iHealth < 1) continue;

		float currentDistance = localPlayer->getDistance(curEnt->vecOrigin);
		if (currentDistance < closestDistance) {
			closestDistance = currentDistance;
			closestDistanceIndex = i;
		}
	}
	if (closestDistanceIndex == -1) {
		return NULL;
	}
	return (Ent*)interfaces::ClientEntityList->GetClientEntity(closestDistanceIndex);
}

void features::doAimbot() {
	if (localPlayer != NULL) {
		if (GetAsyncKeyState(config::aimbotKey)) {
			Ent* bestEnemy = getBestEnemy();
			if (bestEnemy != NULL) {
				aimAt(GetBonePos(bestEnemy, 8));
			}
			/*Ent* closestEnt = GetClosestEnemy();
			if (closestEnt) {
				aimAt(GetBonePos(closestEnt, 8));
			}*/
		}
	}
}



std::string features::calcCurrentKDA() {
	if (PlayerResource == NULL || *PlayerResource <= 0) return "0.00";
	static int* kills = (int*)(*PlayerResource + (killsOffset + 0x4));
	static int* deaths = (int*)(*PlayerResource + (deathsOffset + 0x4));
	static int* assists = (int*)(*PlayerResource + (assistsOffset + 0x4));
	if (kills == NULL || deaths == NULL || assists == NULL) return "0.0";
	float val = 0.0f;
	if (*deaths == 0) {
		val = (float)(*kills + *assists);
	}
	else {
		val = (float)(*kills + *assists) / *deaths;
	}
	std::stringstream stream;	
	stream << std::fixed << std::setprecision(2) << val;
	std::string s = stream.str();
	return s;
}

int features::getPing() {
	if (*PlayerResource <= 0) return 0;
	static int* ping = (int*)(*PlayerResource + (pingOffset + 0x4));
	if (ping == NULL) return 0;
	return *ping;
}

void features::doFov() {
	if (localPlayer != NULL) {
		if (localPlayer->fovAmount != config::fovAmount) {
			localPlayer->fovAmount = config::fovAmount;
		}
	}
}


void features::setupModules() {
	engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
	clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
	clientState = (uintptr_t*)(engineModule + offsets::dwClientState);
	glowObject = (uintptr_t*)(clientModule + offsets::dwGlowObjectManager);
	PlayerResource = (uintptr_t*)(clientModule + offsets::dwPlayerResource);
	localPlayerIndex = (int*)(*clientState + 0x17C);
	killsOffset = GetNetvarOffset("DT_PlayerResource", "m_iKills", interfaces::clientClass);
	deathsOffset = GetNetvarOffset("DT_PlayerResource", "m_iDeaths", interfaces::clientClass);
	assistsOffset = GetNetvarOffset("DT_PlayerResource", "m_iAssists", interfaces::clientClass);
	pingOffset = GetNetvarOffset("DT_PlayerResource", "m_iPing", interfaces::clientClass);
};

void features::reset() {
	if (localPlayer != NULL) {
		localPlayer->fovAmount = 90;
	}
}