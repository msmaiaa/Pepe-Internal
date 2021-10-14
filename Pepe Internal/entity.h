#pragma once
#include "csgo.h"
#include "_vector.h"
#include <Windows.h>

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class Ent {
public:
    union {
        DEFINE_MEMBER_N(vec3, origin, offsets::m_vecOrigin);
        DEFINE_MEMBER_N(int, clientId, offsets::m_iAccountID);
        DEFINE_MEMBER_N(int, m_iHealth, offsets::m_iHealth);
        DEFINE_MEMBER_N(int, m_iArmorValue, offsets::m_ArmorValue);
        DEFINE_MEMBER_N(float, flashDuration, offsets::m_flFlashDuration);
        DEFINE_MEMBER_N(int, fovAmount, offsets::m_iDefaultFOV);
        DEFINE_MEMBER_N(bool, isSpotted, offsets::m_bSpotted);
        DEFINE_MEMBER_N(bool, isDormant, offsets::m_bDormant);
        DEFINE_MEMBER_N(int, shotsFired, offsets::m_iShotsFired);
        DEFINE_MEMBER_N(int, boneMatrix, offsets::m_dwBoneMatrix);
        DEFINE_MEMBER_N(vec3, vecOrigin, offsets::m_vecOrigin);
        DEFINE_MEMBER_N(vec3, aimPunchAngle, offsets::m_aimPunchAngle);
        DEFINE_MEMBER_N(int, teamNum, offsets::m_iTeamNum);
        DEFINE_MEMBER_N(int, glowIndex, offsets::m_iGlowIndex);
        DEFINE_MEMBER_N(int, crosshairId, offsets::m_iCrosshairId);
        DEFINE_MEMBER_N(BYTE, flags, offsets::m_fFlags);
        DEFINE_MEMBER_N(vec3, velocity, offsets::m_vecVelocity);
        DEFINE_MEMBER_N(vec3, m_vecViewOffset, offsets::m_vecViewOffset);
    };
};

class LocalPlayer : public Ent {
public:
    bool resetFlashDuration();
    void aimAt(vec3* target);
    float getDistance(vec3 other);
};

struct GlowStruct {
    BYTE base[8];
    float red;
    float green;
    float blue;
    float alpha;
    BYTE buffer[16];
    bool renderWhenOccluded;
    bool renderWhenNonOccluded;
    bool fullBloom;
    BYTE buffer2[5];
    int glowStyle;
};

