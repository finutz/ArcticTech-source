#pragma once

#include "CBaseEntity.h"
#include "CBaseCombatWeapon.h"
#include "../../Utils/NetVars.h"
#include "../../Utils/Utils.h"
#include <array>
#include "UtlVector.h"

#pragma region DEFINES

#define	FL_ONGROUND				(1 << 0)
#define FL_DUCKING				(1 << 1)
#define	FL_WATERJUMP			(1 << 3)
#define FL_ONTRAIN				(1 << 4)
#define FL_INRAIN				(1 << 5)
#define FL_FROZEN				(1 << 6)
#define FL_ATCONTROLS			(1 << 7)
#define	FL_CLIENT				(1 << 8)
#define FL_FAKECLIENT			(1 << 9)
#define	FL_INWATER				(1 << 10)
#define FL_HIDEHUD_SCOPE		(1 << 11)

enum EMoveType
{
    MOVETYPE_NONE = 0,	// never moves
    MOVETYPE_ISOMETRIC,			// For players -- in TF2 commander view, etc.
    MOVETYPE_WALK,				// Player only - moving on the ground
    MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
    MOVETYPE_FLY,				// No gravity, but still collides with stuff
    MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
    MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
    MOVETYPE_PUSH,				// no clip to world, push and crush
    MOVETYPE_NOCLIP,			// No gravity, no collisions, still do velocity/avelocity
    MOVETYPE_LADDER,			// Used by players only when going onto a ladder
    MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
    MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics

    // should always be defined as the last item in the list
    MOVETYPE_LAST = MOVETYPE_CUSTOM,

    MOVETYPE_MAX_BITS = 4
};

enum LifeState_t {
    LIFE_ALIVE,				 // alive
    LIFE_DYING,				 // playing death animation or still falling off of a ledge waiting to hit ground
    LIFE_DEAD,				 // dead. lying still.
    LIFE_RESPAWNABLE,
    LIFE_DISCARDBODY,
};

enum
{
    EF_BONEMERGE = 0x001,	// Performs bone merge on client side
    EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
    EF_DIMLIGHT = 0x004,	// player flashlight
    EF_NOINTERP = 0x008,	// don't interpolate the next frame
    EF_NOSHADOW = 0x010,	// Don't cast no shadow
    EF_NODRAW = 0x020,	// don't draw entity
    EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
    EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
    // parent and uses the parent's bbox + the max extents of the aiment.
    // Otherwise, it sets up the parent's bones every frame to figure out where to place
    // the aiment, which is inefficient because it'll setup the parent's bones even if
    // the parent is not in the PVS.
    EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
    EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
    EF_MARKED_FOR_FAST_REFLECTION = 0x400,	// marks an entity for reflection rendering when using $reflectonlymarkedentities material variable
    EF_NOSHADOWDEPTH = 0x800,	// Indicates this entity does not render into any shadow depthmap
    EF_SHADOWDEPTH_NOCACHE = 0x1000,	// Indicates this entity cannot be cached in shadow depthmap and should render every frame
    EF_NOFLASHLIGHT = 0x2000,
    EF_NOCSM = 0x4000,	// Indicates this entity does not render into the cascade shadow depthmap
    EF_MAX_BITS = 15
};

// entity flags, CBaseEntity::m_iEFlags
enum
{
    EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
    EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
    EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
    EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
    EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

    EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
    EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

    // The default behavior in ShouldTransmit is to not send an entity if it doesn't
    // have a model. Certain entities want to be sent anyway because all the drawing logic
    // is in the client DLL. They can set this flag and the engine will transmit them even
    // if they don't have a model.
    EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

    EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
    EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
    EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

    // Some dirty bits with respect to abs computations
    EFL_DIRTY_ABSTRANSFORM = (1 << 11),
    EFL_DIRTY_ABSVELOCITY = (1 << 12),
    EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
    EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
    EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
    EFL_HAS_PLAYER_CHILD = (1 << 16),	// One of the child entities is a player.

    EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
    // This forces it to pass the "in PVS" for transmission.
    EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
    EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating

    // FIXME: Not really sure where I should add this...
    EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
    EFL_NO_ROTORWASH_PUSH = (1 << 21),		// I shouldn't be pushed by the rotorwash
    EFL_NO_THINK_FUNCTION = (1 << 22),
    EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),

    EFL_CHECK_UNTOUCH = (1 << 24),
    EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
    EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
    EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
    EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
    EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
    EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
    EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
};

enum EObsMode
{
    OBS_MODE_NONE = 0,	// not in spectator mode
    OBS_MODE_DEATHCAM,	// special mode for death cam animation
    OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
    OBS_MODE_FIXED,		// view from a fixed camera position
    OBS_MODE_IN_EYE,	// follow a player in first person view
    OBS_MODE_CHASE,		// follow a player in third person view
    OBS_MODE_ROAMING,	// free roaming

    NUM_OBSERVER_MODES,
};

enum InvalidatePhysicsBits_t
{
    POSITION_CHANGED = 0x1,
    ANGLES_CHANGED = 0x2,
    VELOCITY_CHANGED = 0x4,
    ANIMATION_CHANGED = 0x8,		// Means cycle has changed, or any other event which would cause render-to-texture shadows to need to be rerendeded
    BOUNDS_CHANGED = 0x10,		// Means render bounds have changed, so shadow decal projection is required, etc.
    SEQUENCE_CHANGED = 0x20,		// Means sequence has changed, only interesting when surrounding bounds depends on sequence																				
};

enum animstate_layer_t
{
    ANIMATION_LAYER_AIMMATRIX = 0,
    ANIMATION_LAYER_WEAPON_ACTION,
    ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
    ANIMATION_LAYER_ADJUST,
    ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
    ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
    ANIMATION_LAYER_MOVEMENT_MOVE,
    ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
    ANIMATION_LAYER_WHOLE_BODY,
    ANIMATION_LAYER_FLASHED,
    ANIMATION_LAYER_FLINCH,
    ANIMATION_LAYER_ALIVELOOP,
    ANIMATION_LAYER_LEAN,
    ANIMATION_LAYER_COUNT,
};

enum PoseParam_t {
    STRAFE_YAW,
    STAND,
    LEAN_YAW,
    SPEED,
    LADDER_YAW,
    LADDER_SPEED,
    JUMP_FALL,
    MOVE_YAW,
    MOVE_BLEND_CROUCH,
    MOVE_BLEND_WALK,
    MOVE_BLEND_RUN,
    BODY_YAW,
    BODY_PITCH,
    AIM_BLEND_STAND_IDLE,
    AIM_BLEND_STAND_WALK,
    AIM_BLEND_STAND_RUN,
    AIM_BLEND_COURCH_IDLE,
    AIM_BLEND_CROUCH_WALK,
    DEATH_YAW
};

#pragma endregion

class CBasePlayer;
class CBaseCombatWeapon;

struct CCSGOPlayerAnimationState
{
    char	pad0[0x60];
    CBasePlayer* pEntity;
    CBaseCombatWeapon* pWeapon;
    CBaseCombatWeapon* pWeaponLast;
    float		flLastUpdateTime;
    int			nLastUpdateFrame;
    float		flLastUpdateIncrement;
    float		flEyeYaw;
    float		flEyePitch;
    float		flFootYaw;
    float		flLastFootYaw;
    float		flMoveYaw;
    float		flMoveYawIdeal;
    float		flMoveYawCurrentToIdeal;
    float       flTimeToAlignLowerBody;
    float		flPrimaryCycle;
    float		flMoveWeight;
    float		flMoveWeightSmoothed;
    float		flDuckAmount;
    float		flDuckAdditional;
    float		flRecrouchWeight;
    Vector		vecOrigin;
    Vector		vecLastOrigin;
    Vector		vecVelocity;
    Vector		vecVelocityNormalized;
    Vector		vecVelocityNormalizedNonZero;
    float		flVelocityLenght2D;
    float		flVelocityZ;
    float		flRunSpeedNormalized;
    float		flWalkSpeedNormalized;
    float		flCrouchSpeedNormalized;
    float		flDurationMoving;
    float		flDurationStill;
    bool		bOnGround;
    bool		bLanding;
    float       flJumpToFall;
    float		flDurationInAir;
    float		flLeftGroundHeight;
    float		flHitGroundWeight;
    float		flWalkToRunTransition;
    char	    __pad3[0x4];
    float		flInAirSmoothValue;
    bool        bOnLadder;
    float       flLadderWeights;
    float       flLadderSpeed;
    bool        bWalkToRunTransitionState;
    bool        bDefuseStarted;
    bool        bPlantAnimStarted;
    bool        bTwitchAnimStarted;
    bool        bAdjustStarted;
    char        vecActivityModifiers[20];
    float       flNextTwitchTime;
    float       flTimeOfLastKnownInjury;
    float       flLastVelocityTestTime;
    Vector      vecVelocityLast;
    Vector      vecTargetAcceleration;
    Vector      vecAcceleration;
    float       flAccelerationWeight;
    float       flAimMatrixTransition;
    float       flAimMatrixTransitionDelay;
    bool        bFlashed;
    float       flStrafeChangeWeight;
    float       flStrafeChangeTargetWeight;
    float       flStrafeChangeCycle;
    int         nStrafeSequence;
    bool        bStrafeChanging;
    float       flDurationStrafing;
    float       flFootLerp;
    bool        bFeetCrossed;
    bool        bPlayerIsAccelerating;
    char        __pad4[0x178];
    float       flCameraSmoothHeight;
    bool        bSmoothHeightValid;
    float		flLastTimeVelocityOverTen;
    float		flAimYawMin;
    float		flAimYawMax;
    float		flAimPitchMin;
    float		flAimPitchMax;
    int         iAnimsetVersion;

    float& YawDesyncAdjustement()
    {
        return *(float*)((uintptr_t)this + 0x334);
    }

    void SetTickInterval() {
        if (nLastUpdateFrame > GlobalVars->framecount - 1)
            nLastUpdateFrame = GlobalVars->framecount - 1;

        if (flLastUpdateTime >= GlobalVars->curtime)
            flLastUpdateTime = GlobalVars->curtime - GlobalVars->interval_per_tick;
    }
};
static_assert(sizeof(CCSGOPlayerAnimationState) == 0x348);

struct AnimationLayer {
    bool m_bClientBlend;		 //0x0000
    float m_flBlendIn;			 //0x0004
    void* m_pStudioHdr;			 //0x0008
    int m_nDispatchSequence;     //0x000C
    int m_nDispatchSequence_2;   //0x0010
    uint32_t m_nOrder;           //0x0014
    uint32_t m_nSequence;        //0x0018
    float m_flPrevCycle;       //0x001C
    float m_flWeight;          //0x0020
    float m_flWeightDeltaRate; //0x0024
    float m_flPlaybackRate;    //0x0028
    float m_flCycle;           //0x002C
    CBasePlayer* m_pOwner;       //0x0030
    char pad_0038[4];            //0x0034
};

class CUserCmd;
class C_BaseAnimating;


class CBoneAccessor
{
public:
    C_BaseAnimating* m_pAnimating;

    matrix3x4_t* m_pBones;

    int m_ReadableBones;		// Which bones can be read.
    int m_WritableBones;		// Which bones can be written.
};



#define MAX_WEAPONS 64
#define MAX_VIEWMODELS 2
#define INVALID_EHANDLE_INDEX	0xFFFFFFFF

class CBasePlayer : public CBaseEntity {
public:
    NETVAR(m_iHealth, int, "DT_BasePlayer", "m_iHealth")
    NETVAR(m_fFlags, int, "DT_CSPlayer", "m_fFlags")
    NETVAR(m_vecViewOffset, Vector, "DT_BasePlayer", "m_vecViewOffset[0]")
    NETVAR(m_iTeamNum, int, "DT_BaseEntity", "m_iTeamNum")
    NETVAR(m_bIsScoped, bool, "DT_CSPlayer", "m_bIsScoped")
    NETVAR(m_bResumeZoom, bool, "DT_CSPlayer", "m_bResumeZoom")
    NETVAR_O(m_MoveType, int, "DT_BaseEntity", "m_nRenderMode", 1)
    NETVAR(m_flDuckAmount, float, "DT_BasePlayer", "m_flDuckAmount")
    NETVAR(m_flDuckSpeed, float, "DT_BasePlayer", "m_flDuckSpeed")
    NETVAR(m_ArmorValue, int, "DT_CSPlayer", "m_ArmorValue")
    NETVAR(m_bHasHelmet, bool, "DT_CSPlayer", "m_bHasHelmet")
    NETVAR(m_bIsDefusing, bool, "DT_CSPlayer", "m_bIsDefusing")
    NETVAR(m_hActiveWeapon, unsigned long, "DT_BaseCombatCharacter", "m_hActiveWeapon")
    PNETVAR_O(m_hMyWeapons, unsigned long, "DT_BaseCombatCharacter", "m_hActiveWeapon", -256)
    NETVAR(m_nTickBase, int, "DT_BasePlayer", "m_nTickBase")
    NETVAR(m_lifeState, int, "DT_BasePlayer", "m_lifeState")
    NETVAR(m_iShotsFired, int, "DT_CSPlayer", "m_iShotsFired")
    NETVAR(m_nHitboxSet, int, "DT_BasePlayer", "m_nHitboxSet")
    NETVAR(m_angEyeAngles, QAngle, "DT_CSPlayer", "m_angEyeAngles")
    NETVAR(m_viewPunchAngle, QAngle, "DT_BasePlayer", "m_viewPunchAngle")
    NETVAR(m_aimPunchAngle, QAngle, "DT_BasePlayer", "m_aimPunchAngle")
    NETVAR(m_aimPunchAngleVel, QAngle, "DT_BasePlayer", "m_aimPunchAngleVel")
    NETVAR(m_flNextAttack, float, "DT_BaseCombatCharacter", "m_flNextAttack")
    NETVAR(m_flVelocityModifier, float, "DT_CSPlayer", "m_flVelocityModifier")
    NETVAR(m_flFlashDuration, float, "DT_CSPlayer", "m_flFlashDuration")
    NETVAR(m_hObserverTarget, unsigned long, "DT_BasePlayer", "m_hObserverTarget")
    NETVAR(m_flLowerBodyYawTarget, float, "DT_CSPlayer", "m_flLowerBodyYawTarget")
    NETVAR(m_iObserverMode, int, "DT_BasePlayer", "m_iObserverMode")
    NETVAR(m_nSequence, int, "DT_BaseAnimating", "m_nSequence")
    NETVAR(m_flThirdpersonRecoil, float, "DT_CSPlayer", "m_flThirdpersonRecoil")
    NETVAR(m_bStrafing, bool, "DT_CSPlayer", "m_bStrafing")
    NETVAR(m_nNextThinkTick, int, "DT_BasePlayer", "m_nNextThinkTick")
    NETVAR(m_flCycle, float, "DT_BaseAnimating", "m_flCycle")
    NETVAR(m_bGunGameImmunity, bool, "DT_CSPlayer", "m_bGunGameImmunity")
    NETVAR( m_iAddonBits, int, "DT_CSPlayer", "m_iAddonBits" )
    OFFSET(m_flFallVelocity, float, 0x58)
    OFFSET(m_nSimulationTick, int, 0x2AC)
    NETVAR_O(m_nFinalPredictedTick, int, "DT_CSPlayer", "m_nTickBase", 0x4)
    NETVAR(m_bClientSideAnimation, bool, "DT_BaseAnimating", "m_bClientSideAnimation")
    NETVAR(m_hVehicle, unsigned long, "DT_BasePlayer", "m_hVehicle")
    PNETVAR(m_hViewModel, unsigned long, "DT_BasePlayer", "m_hViewModel[0]")
    PPRED_DESC_MAP(m_nButtons, int, "m_nButtons")
    PRED_DESC_MAP(m_afButtonsLast, int, "m_afButtonLast")
    PRED_DESC_MAP(m_afButtonsPressed, int, "m_afButtonPressed")
    PRED_DESC_MAP(m_afButtonsReleased, int, "m_afButtonReleased")
    PRED_DESC_MAP(m_fEffects, int, "m_fEffects")
    PRED_DESC_MAP(m_vecAbsVelocity, Vector, "m_vecAbsVelocity")
    OFFSET(m_bDormant, bool, 0xED)
    NETVAR_O(GetAnimstate, CCSGOPlayerAnimationState*, "DT_CSPlayer", "m_bIsScoped", -0x14)
    NETVAR(deadflag, bool, "DT_BasePlayer", "deadflag")
    NETVAR_O(v_angle, QAngle, "DT_BasePlayer", "deadflag", 0x4)
    OFFSET(m_nOcclusionFlags, uint32_t, 0xA28)
    OFFSET(m_nOcclusionFrame, int, 0xA30)
    OFFSET(m_BoneAccessor, CBoneAccessor, 0x26a4)
    NETVAR(m_bIsWalking, bool, "DT_CSPlayer", "m_bIsWalking")
    NETVAR_O(m_iMostRecentModelBoneCounter, unsigned long, "DT_BaseAnimating", "m_nForceBone", 0x4)
    PRED_DESC_MAP(m_nSkin, int, "m_nSkin")
    PRED_DESC_MAP(m_nBody, int, "m_nBody")
    NETVAR_O(m_surfaceFriction, float, "DT_BasePlayer", "m_flWaterJumpTime", 17)
    NETVAR(m_iAccount, int, "DT_CSPlayer", "m_iAccount")
    PRED_DESC_MAP(m_vecNetworkOrigin, Vector, "m_vecNetworkOrigin")

    bool& m_bMaintainSequenceTransitions();

    void SetModelIndex( int modelIndex );

    inline bool IsArmored(const int iHitGroup)
    {
        // @ida isarmored: server.dll @ 55 8B EC 32 D2

        bool bIsArmored = false;

        if (this->m_ArmorValue() > 0)
        {
            switch (iHitGroup)
            {
            case HITGROUP_GENERIC:
            case HITGROUP_CHEST:
            case HITGROUP_STOMACH:
            case HITGROUP_LEFTARM:
            case HITGROUP_RIGHTARM:
            case HITGROUP_NECK:
                bIsArmored = true;
                break;
            case HITGROUP_HEAD:
                if (this->m_bHasHelmet())
                    bIsArmored = true;
                [[fallthrough]];
            case HITGROUP_LEFTLEG:
            case HITGROUP_RIGHTLEG:
                break;
            default:
                break;
            }
        }

        return bIsArmored;
    }

    void SetSequence(int iSequence)
    {
        CallVFunction<void(__thiscall*)(CBasePlayer*, int)>(this, 219)(this, iSequence);
    }

    void StudioFrameAdvance()
    {
        CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 220)(this);
    }

    void PreThink()
    {
        CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 318)(this);
    }

    void UpdateCollisionBounds()
    {
        CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 340)(this);
    }

    void InvalidatePhysicsRecursive(int flags) {
        static auto m_uInvalidatePhysics = Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04");
        reinterpret_cast <void(__thiscall*)(void*, int)> (m_uInvalidatePhysics)(this, flags);
    }

    std::string         GetName();
    bool                IsTeammate();
    bool                IsEnemy();
    float               GetMaxDesyncDelta();
    bool                IsAlive();
    Vector              GetBonePosition(int bone);
    Vector              GetHitboxCenter(int hitbox, matrix3x4_t* matrix = nullptr);
    CUtlVector<matrix3x4_t> GetCachedBoneData();
    float               ScaleDamage(int hitgroup, CCSWeaponData* weaponData, float& damage);

    Vector              GetEyePosition();
    void                ModifyEyePosition(Vector& eye_position);
    Vector              GetShootPosition();
    void                UpdateClientSideAnimation();
    void                UpdateAnimationState(CCSGOPlayerAnimationState* state, const QAngle& angles, bool bForce = false);
    AnimationLayer*     GetAnimlayers();
    CBaseCombatWeapon*  GetActiveWeapon();
    bool                SetupBones(matrix3x4_t* boneToWorld, int maxBones, int mask, float curTime = 0.f);
    void                SetupBones_AttachmentHelper();
    std::array<float, 24>& m_flPoseParameter();
    C_CommandContext*   GetCommandContext(); 
    void                SetAbsVelocity(const Vector& vecAbsVelocity);
    float               GetMaxSpeed();
    void                CopyBones(matrix3x4_t* boneMatrix);
    void                ClampBonesInBBox(matrix3x4_t* bones, int bone_mask);

    int&                m_nImpulse();
    int                 GetButtonForced();
    int                 GetButtonDisabled();
    CUserCmd**          GetCurrentCommand();
    CUserCmd&           GetLastCommand();
    void                Think();
    bool                PhysicsRunThink(int nThinkMetod);
    void                PostThink();
    void                SelectItem(const char* string, int subtype);
    bool                UsingStandardWeaponsInVehicle();
    void                UpdateButtonState(int button);
    bool                IsHitboxArmored(int hitbox);

    void                DrawServerHitboxes(float duration, bool monoColor);

    float& m_flLastBoneSetupTime();
    void InvalidateBoneCache();
    void ForceBoneCache();
    CStudioHdr* GetStudioHdr();
};