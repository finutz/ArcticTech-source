#pragma once

#include <string>

#include "../Misc/Vector.h"
#include "../Misc/Matrix.h"

#pragma region MASKS

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1       
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40 
#define   CONTENTS_OPAQUE               0x80 
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200     
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800 
#define   CONTENTS_TEAM2                0x1000 
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000 

#define   CONTENTS_MONSTER              0x2000000 
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000 
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001 
#define   SURF_SKY2D                    0x0002 
#define   SURF_SKY                      0x0004 
#define   SURF_WARP                     0x0008 
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020 
#define   SURF_TRIGGER                  0x0040 
#define   SURF_NODRAW                   0x0080 

#define   SURF_HINT                     0x0100 

#define   SURF_SKIP                     0x0200   
#define   SURF_NOLIGHT                  0x0400   
#define   SURF_BUMPLIGHT                0x0800   
#define   SURF_NOSHADOWS                0x1000   
#define   SURF_NODECALS                 0x2000   
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000   
#define   SURF_HITBOX                   0x8000   

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#pragma endregion

#pragma region valve_decals
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E'
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_SNOW			'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_REFLECTIVE		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z'
#define CHAR_TEX_CLAY			1 	// L4D addition
#define CHAR_TEX_PLASTER		2	// L4D addition
#define CHAR_TEX_ROCK			3	// L4D addition
#define CHAR_TEX_RUBBER			4	// L4D addition
#define CHAR_TEX_SHEETROCK		5	// L4D addition
#define CHAR_TEX_CLOTH			6	// L4D addition
#define CHAR_TEX_CARPET			7	// L4D addition
#define CHAR_TEX_PAPER			8	// L4D addition
#define CHAR_TEX_UPHOLSTERY		9	// L4D addition
#define CHAR_TEX_PUDDLE			10	// L4D addition
#define CHAR_TEX_MUD			11	// L4D addition
#pragma endregion

enum EHitGroupIndex : int
{
    HITGROUP_GENERIC = 0,
    HITGROUP_HEAD,
    HITGROUP_CHEST,
    HITGROUP_STOMACH,
    HITGROUP_LEFTARM,
    HITGROUP_RIGHTARM,
    HITGROUP_LEFTLEG,
    HITGROUP_RIGHTLEG,
    HITGROUP_NECK,
    HITGROUP_GEAR = 10
};

inline std::string GetHitgroupName(int hg) {
    static std::string hg_to_s[]{
        "generic",
        "head",
        "chest",
        "stomach",
        "arm",
        "arm",
        "leg",
        "leg",
        "neck",
        "gear"
    };

    return hg_to_s[hg];
}

enum Collision_Group_t
{
    COLLISION_GROUP_NONE = 0,
    COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
    COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
    COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
    COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
    COLLISION_GROUP_NPC,			// Generic NPC group
    COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
    COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
    COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
    COLLISION_GROUP_PROJECTILE,		// Projectiles!
    COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
    COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
    COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
    COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code
    COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
    COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other
    COLLISION_GROUP_PZ_CLIP,
    COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE, // Only collides with bullets
    LAST_SHARED_COLLISION_GROUP
};

typedef unsigned short MaterialHandle_t;

struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;
class ITraceListData;
class CPhysCollide;
class IHandleEntity;
class ICollideable;
class CBaseEntity;
struct cplane_t
{
    Vector normal;
    float dist;
    uint8_t type;   // for fast side tests
    uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
    uint8_t pad[2];

};
struct virtualmeshlist_t;
class IClientEntity;

enum class TraceType
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,
    TRACE_ENTITIES_ONLY,
    TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* pEntity, int contentsMask) = 0;
    virtual TraceType GetTraceType() const = 0;
};

//-----------------------------------------------------------------------------
// Classes are expected to inherit these + implement the ShouldHitEntity method
//-----------------------------------------------------------------------------

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
    CTraceFilter() {}

    CTraceFilter(const CBaseEntity* ent) {
        pSkip = (void*)ent;
    }

    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == pSkip);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
    void* pSkip;
};

class CTraceFilterSkipEntity : public ITraceFilter
{
public:
    CTraceFilterSkipEntity(IHandleEntity* pEntityHandle)
    {
        pSkip = pEntityHandle;
    }

    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == pSkip);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
    void* pSkip;
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return true;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }
};


//-----------------------------------------------------------------------------
// Classes need not inherit from these
//-----------------------------------------------------------------------------
class CTraceFilterWorldOnly : public ITraceFilter
{
public:
    bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_WORLD_ONLY;
    }
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return false;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }
};

class CTraceFilterPlayersOnlySkipOne : public ITraceFilter
{
public:
    CTraceFilterPlayersOnlySkipOne(IClientEntity* ent)
    {
        pEnt = ent;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return pEntityHandle != (IHandleEntity*)pEnt;
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_ENTITIES_ONLY;
    }

private:
    IClientEntity* pEnt;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
    CTraceFilterSkipTwoEntities(IClientEntity* ent1, IClientEntity* ent2)
    {
        pEnt1 = ent1;
        pEnt2 = ent2;
    }
    bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
    {
        return !(pEntityHandle == (IHandleEntity*)pEnt1 || pEntityHandle == (IHandleEntity*)pEnt2);
    }
    virtual TraceType GetTraceType() const
    {
        return TraceType::TRACE_EVERYTHING;
    }

private:
    IClientEntity* pEnt1;
    IClientEntity* pEnt2;
};

class CTraceFilterHitAll : public CTraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
    {
        return true;
    }
};

class CTraceFilterSimple : public CTraceFilter
{
public:
    virtual bool ShouldHitEntity(IHandleEntity* pHandleEntity, int contentsMask) { return false; };
    virtual void SetPassEntity(void* pPassEntity) { m_pPassEnt = pPassEntity; }
    virtual void SetCollisionGroup(int iCollisionGroup) { m_collisionGroup = iCollisionGroup; }

    void* GetPassEntity(void) { return m_pPassEnt; }

private:
    void* m_pPassEnt;
    int m_collisionGroup;

};


enum class DebugTraceCounterBehavior_t
{
    kTRACE_COUNTER_SET = 0,
    kTRACE_COUNTER_INC,
};

//-----------------------------------------------------------------------------
// Enumeration interface for EnumerateLinkEntities
//-----------------------------------------------------------------------------
class IEntityEnumerator
{
public:
    // This gets called with each handle
    virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};


struct BrushSideInfo_t
{
    Vector4D plane;               // The plane of the brush side
    unsigned short bevel;    // Bevel plane?
    unsigned short thin;     // Thin?
};

class CPhysCollide;

struct vcollide_t
{
    unsigned short solidCount : 15;
    unsigned short isPacked : 1;
    unsigned short descSize;
    // VPhysicsSolids
    CPhysCollide** solids;
    char* pKeyValues;
    void* pUserData;
};

struct cmodel_t
{
    Vector         mins, maxs;
    Vector         origin;        // for sounds or lights
    int            headnode;
    vcollide_t     vcollisionData;
};

struct csurface_t
{
    const char* name;
    short          surfaceProps;
    unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
    VectorAligned  m_Start;  // starting point, centered within the extents
    VectorAligned  m_Delta;  // direction + length of the ray
    VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
    VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
    const matrix3x4_t* m_pWorldAxisTransform;
    bool m_IsRay;  // are the extents zero?
    bool m_IsSwept;     // is delta != 0?

    Ray_t() : m_pWorldAxisTransform(NULL) {}

    Ray_t(Vector const& start, Vector const& end) {
        Init(start, end);
    }

    Ray_t(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs) {
        Init(start, end, mins, maxs);
    }

    void Init(Vector const& start, Vector const& end)
    {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents.Init();

        m_pWorldAxisTransform = NULL;
        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.Init();
        m_Start = start;
    }

    void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
    {
        m_Delta = end - start;

        m_pWorldAxisTransform = NULL;
        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents = maxs - mins;
        m_Extents *= 0.5f;
        m_IsRay = (m_Extents.LengthSqr() < 1e-6);

        // Offset m_Start to be in the center of the box...
        m_StartOffset = maxs + mins;
        m_StartOffset *= 0.5f;
        m_Start = start + m_StartOffset;
        m_StartOffset *= -1.0f;
    }
    Vector InvDelta() const
    {
        Vector vecInvDelta;
        for (int iAxis = 0; iAxis < 3; ++iAxis) {
            if (m_Delta[iAxis] != 0.0f) {
                vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
            }
            else {
                vecInvDelta[iAxis] = FLT_MAX;
            }
        }
        return vecInvDelta;
    }

private:
};

class CBaseTrace
{
public:
    bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
    bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
    bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
    bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
    bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

    // these members are aligned!!
    Vector         startpos;            // start position
    Vector         endpos;              // final position
    cplane_t       plane;               // surface normal at impact

    float          fraction;            // time completed, 1.0 = didn't hit anything

    int            contents;            // contents on other side of surface hit
    unsigned short dispFlags;           // displacement flags for marking surfaces with data

    bool           allsolid;            // if true, plane is not valid
    bool           startsolid;          // if true, the initial point was in a solid area

    CBaseTrace() {}

};

class CGameTrace : public CBaseTrace
{
public:
    bool DidHitWorld() const;
    bool DidHitNonWorldEntity() const;
    int GetEntityIndex() const;
    bool DidHit() const;
    bool IsVisible() const;

public:

    float               fractionleftsolid;  // time we left a solid, only valid if we started in solid
    csurface_t          surface;            // surface hit (impact surface)
    int                 hitgroup;           // 0 == generic, non-zero is specific body part
    short               physicsbone;        // physics bone hit by trace in studio
    unsigned short      worldSurfaceIndex;  // Index of the msurface2_t, if applicable
    CBaseEntity* hit_entity;
    int                 hitbox;                       // box hit by trace in studio

    CGameTrace() {}

    CGameTrace& operator=(const CGameTrace& other)
    {
        startpos = other.startpos;
        endpos = other.endpos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        dispFlags = other.dispFlags;
        allsolid = other.allsolid;
        startsolid = other.startsolid;
        fractionleftsolid = other.fractionleftsolid;
        surface = other.surface;
        hitgroup = other.hitgroup;
        physicsbone = other.physicsbone;
        worldSurfaceIndex = other.worldSurfaceIndex;
        hit_entity = other.hit_entity;
        hitbox = other.hitbox;
        return *this;
    }

    CGameTrace(const CGameTrace& other) :
        fractionleftsolid(other.fractionleftsolid),
        surface(other.surface),
        hitgroup(other.hitgroup),
        physicsbone(other.physicsbone),
        worldSurfaceIndex(other.worldSurfaceIndex),
        hit_entity(other.hit_entity),
        hitbox(other.hitbox)
    {
        startpos = other.startpos;
        endpos = other.endpos;
        plane = other.plane;
        fraction = other.fraction;
        contents = other.contents;
        dispFlags = other.dispFlags;
        allsolid = other.allsolid;
        startsolid = other.startsolid;
    }

    void Clear() {
        memset(this, 0, sizeof(CGameTrace));
        fraction = 1.f;
        fractionleftsolid = 0;
        surface = { "**empty**", 0 };
    }
};

inline bool CGameTrace::DidHit() const
{
    return fraction < 1 || allsolid || startsolid;
}

inline bool CGameTrace::IsVisible() const
{
    return fraction > 0.97f;
}

class CBasePlayer;

class IEngineTrace
{
public:
    virtual int     GetPointContents(const Vector& vecAbsPosition, int contentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
    virtual int     GetPointContents_WorldOnly(const Vector& vecAbsPosition, int contentsMask = MASK_ALL) = 0;
    virtual int     GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;
    virtual void    ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEnt, CGameTrace* pTrace) = 0;
    virtual void    ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, CGameTrace* pTrace) = 0;
    virtual void    TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, CGameTrace* pTrace) = 0;

    bool            ClipRayToHitboxes(const Ray_t& ray, unsigned int fMask, CBaseEntity* pEnt, CGameTrace* pTrace);
    bool            ClipRayToPlayer(const Ray_t& ray, unsigned int fMask, CBaseEntity* pEnt, CGameTrace* pTrace);
    bool            RayIntersectPlayer(const Vector& start, const Vector& end, CBasePlayer* player, matrix3x4_t* hitbox, int filter_damagegroup = -1);

    CGameTrace      TraceRay(const Vector& start, const Vector& end, unsigned int fMask, CBaseEntity* pSkip = nullptr) {
        CGameTrace trace;
        CTraceFilter filter;
        Ray_t ray(start, end);
        filter.pSkip = pSkip;

        this->TraceRay(ray, fMask, &filter, &trace);

        return trace;
    }

    CGameTrace      TraceHull(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, unsigned int fMask, CBaseEntity* pSkip = nullptr) {
        CGameTrace trace;
        CTraceFilter filter;
        Ray_t ray;
        ray.Init(start, end, mins, maxs);
        filter.pSkip = pSkip;

        this->TraceRay(ray, fMask, &filter, &trace);

        return trace;
    }

    // find nearest point to line
    Vector          ClosestPoint(const Vector& start, const Vector& end, const Vector& point);
    // calculates distance between line and point
    float           DistanceToRay(const Vector& start, const Vector& end, const Vector& point);
    // segment to segment distance
    float           SegmentToSegment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2);

    bool            IntersectBBHitbox(const Vector& start, const Vector& delta, const Vector& min, const Vector& max);
};