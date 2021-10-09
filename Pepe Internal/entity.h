#pragma once
#include "csgo.h"
#include "vector.h"

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
        DEFINE_MEMBER_N(bool, isSpotted, offsets::m_bSpotted);
        DEFINE_MEMBER_N(bool, isDormant, offsets::m_bDormant);
        DEFINE_MEMBER_N(vec3, m_vecViewOffset, offsets::m_vecViewOffset);
    };
};

class LocalPlayer : public Ent {
public:
    bool resetFlashDuration();
};

class IClientEntityList
{
public:
    // Get IClientNetworkable interface for specified entity
    virtual void* GetClientNetworkable(int entnum) = 0;
    virtual void* GetClientNetworkableFromHandle(int hEnt) = 0;
    virtual void** GetClientUnknownFromHandle(int hEnt) = 0;

    // NOTE: This function is only a convenience wrapper.
    // It returns GetClientNetworkable( entnum )->GetIClientEntity().
    virtual void* GetClientEntity(int entnum) = 0;
    virtual void* GetClientEntityFromHandle(int hEnt) = 0;

    // Returns number of entities currently in use
    virtual int                    NumberOfEntities(bool bIncludeNonNetworkable) = 0;

    // Returns highest index actually used
    virtual int                    GetHighestEntityIndex(void) = 0;

    // Sizes entity list to specified size
    virtual void                SetMaxEntities(int maxents) = 0;
    virtual int                    GetMaxEntities() = 0;
};