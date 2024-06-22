#pragma once
#include "../../Utils/VitualFunction.h"
#include "IEngineTrace.h"

class CBaseEntity;
class IClientNetworkable;
class ClientClass;
class CCSWeaponData;

enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

class IClientEntityList
{
public:
    virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
    virtual IClientNetworkable*   GetClientNetworkableFromHandle(unsigned long hEnt) = 0;
    virtual void*                 GetClientUnknownFromHandle(unsigned long hEnt) = 0;
    virtual CBaseEntity*          GetClientEntity(int entNum) = 0;
    virtual CBaseEntity*          GetClientEntityFromHandle(unsigned long hEnt) = 0;
    virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
    virtual int                   GetHighestEntityIndex(void) = 0;
    virtual void                  SetMaxEntities(int maxEnts) = 0;
    virtual int                   GetMaxEntities() = 0;
};

class IWeaponSystem {
public:
    CCSWeaponData* GetWeaponData(short index) {
        return CallVFunction<CCSWeaponData* (__thiscall*)(IWeaponSystem*, short)>(this, 2)(this, index);
    }
};

void* UTIL_GetServerPlayer(int playerIndex);