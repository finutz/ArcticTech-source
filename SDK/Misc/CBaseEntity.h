#pragma once

#include "../Interfaces.h"
#include "../../Utils/NetVars.h"
#include "UtlVector.h"

#define NETVAR(func, type, table, netvar)								\
	type& func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar);	\
		return *(type*)((uintptr_t)this + _##func);				\
	}

#define PNETVAR(func, type, table, netvar)								\
	type* func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar);	\
		return (type*)((uintptr_t)this + _##func);				\
	}	

#define NETVAR_O(func, type, table, netvar, offset)								\
	type& func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar) + offset;	\
		return *(type*)((uintptr_t)this + _##func);				\
	}	

#define PNETVAR_O(func, type, table, netvar, offset)								\
	type* func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar) + offset;	\
		return (type*)((uintptr_t)this + _##func);				\
	}	


#define OFFSET(func, type, offset)								\
	type& func##() {								\
		return *(type*)((uintptr_t)this + offset);				\
	}	

#define PRED_DESC_MAP(func, type, name) \
	type& func##(){  \
		static int _##func = NetVars::FindInDataMap(GetPredDescMap(), name); \
		return *(type*)(this + _##func); \
	}

#define PPRED_DESC_MAP(func, type, name) \
	type* func##(){  \
		static int _##func = NetVars::FindInDataMap(GetPredDescMap(), name); \
		return (type*)(this + _##func); \
	}

class CBasePlayer;

typedef unsigned int CBaseHandle;

class VarMapEntry_t
{
public:
	unsigned short		type;
	unsigned short		m_bNeedsToInterpolate;	// Set to false when this var doesn't
	// need Interpolate() called on it anymore.
	void* data;
	void* watcher;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	CUtlVector< VarMapEntry_t >	m_Entries;
	int							m_nInterpolatedEntries;
	float						m_lastInterpolationTime;
};

class IHandleEntity
{
public:
	virtual							~IHandleEntity() { }
	virtual void					SetRefEHandle(const CBaseHandle& hRef) = 0;
	virtual const CBaseHandle&		GetRefEHandle() const = 0;
};

class ICollideable
{
public:
	// Gets at the entity handle associated with the collideable
	virtual IHandleEntity* GetEntityHandle() = 0;

	// These methods return the bounds of an OBB measured in "collision" space
	// which can be retreived through the CollisionToWorldTransform or
	// GetCollisionOrigin/GetCollisionAngles methods
	virtual Vector& OBBMins() const = 0;
	virtual Vector& OBBMaxs() const = 0;

	// Returns the bounds of a world-space box used when the collideable is being traced
	// against as a trigger. It's only valid to call these methods if the solid flags
	// have the FSOLID_USE_TRIGGER_BOUNDS flag set.
	virtual void			WorldSpaceTriggerBounds(Vector* pVecWorldMins, Vector* pVecWorldMaxs) const = 0;

	// custom collision test
	virtual bool			TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Perform hitbox test, returns true *if hitboxes were tested at all*!!
	virtual bool			TestHitboxes(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Returns the BRUSH model index if this is a brush model. Otherwise, returns -1.
	virtual int				GetCollisionModelIndex() = 0;

	// Return the model, if it's a studio model.
	virtual const model_t* GetCollisionModel() = 0;

	// Get angles and origin.
	virtual const Vector& GetCollisionOrigin() const = 0;
	virtual const QAngle& GetCollisionAngles() const = 0;
	virtual const matrix3x4_t& CollisionToWorldTransform() const = 0;

	// Return a SOLID_ define.
	virtual SolidType_t		GetSolid() const = 0;
	virtual int				GetSolidFlags() const = 0;

	// Gets at the containing class...
	virtual void* GetIClientUnknown() = 0;

	// We can filter out collisions based on collision group
	virtual int				GetCollisionGroup() const = 0;

	// Returns a world-aligned box guaranteed to surround *everything* in the collision representation
	// Note that this will surround hitboxes, trigger bounds, physics.
	// It may or may not be a tight-fitting box and its volume may suddenly change
	virtual void			WorldSpaceSurroundingBounds(Vector* pVecMins, Vector* pVecMaxs) = 0;

	virtual unsigned int	GetRequiredTriggerFlags() const = 0;

	// returns NULL unless this collideable has specified FSOLID_ROOT_PARENT_ALIGNED
	virtual const matrix3x4_t* GetRootParentToWorldTransform() const = 0;
	virtual void* GetVPhysicsObject() const = 0;
};

class IClientThinkable;
class IClientAlphaProperty;
class IClientUnknown;
class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector& GetRenderOrigin() = 0;
	virtual QAngle& GetRenderAngles() = 0;
	virtual bool ShouldDraw() = 0;
	virtual int GetRenderFlags() = 0;
	virtual bool IsTransparent() = 0;
	virtual void GetShadowHandle() const = 0;
	virtual void* RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int DrawModel(int nFlags, const void* instance) = 0;
	virtual int GetBody() = 0;
	virtual void GetColorModulation(float* arrColor) = 0;
	virtual bool LODTest() = 0;
	virtual bool SetupBones(matrix3x4_t* arrBonesToWorld, int iMaxBones, int nBoneMask, float flCurrentTime) = 0;
	virtual void SetupWeights(const matrix3x4_t* arrBonesToWorld, int nFlexWeightCount, float* arrFlexWeight, float* arrFlexDelayedWeight) = 0;
	virtual void DoAnimationEvents() = 0;
	virtual void* GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector& vecMins, Vector& vecMaxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector& vecMins, Vector& vecMaxs) = 0;
	virtual void GetShadowRenderBounds(Vector& vecMins, Vector& vecMaxs, int nShadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int nFlags) = 0;
	virtual bool GetShadowCastDistance(float* pflDistance, int iShadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector* vecDirection, int iShadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int ShadowCastType() = 0;
	virtual void unused2() { }
	virtual void CreateModelInstance() = 0;
	virtual void* GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* szAttachmentName) = 0;
	virtual bool GetAttachment(int nIndex, Vector& vecOrigin, QAngle& angView) = 0;
	virtual bool GetAttachment(int nIndex, matrix3x4_t& matAttachment) = 0;
	virtual bool ComputeLightingOrigin(int nAttachmentIndex, Vector vecModelLightingCenter, const matrix3x4_t& matrix, Vector& vecTransformedLightingCenter) = 0;
	virtual float* GetRenderClipPlane() = 0;
	virtual int GetSkin() = 0;
	virtual void OnThreadedDrawSetup() = 0;
	virtual bool UsesFlexDelayedWeights() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual std::uint8_t OverrideAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual std::uint8_t OverrideShadowAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual void* GetClientModelRenderable() = 0;
};

class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual CBaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*			GetIClientUnknown() = 0;
	virtual void					Release() = 0;
	virtual ClientClass*			GetClientClass() = 0;
	virtual void					NotifyShouldTransmit(int iState) = 0;
	virtual void					OnPreDataChanged(int updateType) = 0;
	virtual void					OnDataChanged(int updateType) = 0;
	virtual void					PreDataUpdate(int updateType) = 0;
	virtual void					PostDataUpdate(int updateType) = 0;
	virtual void					OnDataUnchangedInPVS() = 0;
	virtual bool					IsDormant() const = 0;
	virtual int						GetIndex() const = 0;
	virtual void					ReceiveMessage(int classIndex, void* msg) = 0;
	virtual void*					GetDataTableBasePtr() = 0;
	virtual void					SetDestroyedOnRecreateEntities() = 0;
};

class CBaseEntity {
public:
	NETVAR(m_vecOrigin, Vector, "DT_BaseEntity", "m_vecOrigin")
	NETVAR(m_vecVelocity, Vector, "DT_BasePlayer", "m_vecVelocity[0]")
	NETVAR(m_nModelIndex, int, "DT_BaseEntity", "m_nModelIndex")
	PRED_DESC_MAP(m_iEFlags, int, "m_iEFlags")
	OFFSET(EntIndex, int, 0x64)
	OFFSET(m_EntClientFlags, unsigned short, 0x68)
	NETVAR_O(m_flSpawnTime, float, "DT_BaseEntity", "m_flUseLookAtAngle", 0x5)
	NETVAR_O(m_isJiggleBonesEnabled, bool, "DT_BaseAnimating", "m_hLightingOrigin", -0x18)
	NETVAR(moveparent, unsigned long, "DT_BaseEntity", "moveparent")
	NETVAR(m_hOwnerEntity, unsigned long, "DT_BaseEntity", "m_hOwnerEntity")
	NETVAR(m_vecMins, Vector, "DT_BaseEntity", "m_vecMins")
	NETVAR(m_vecMaxs, Vector, "DT_BaseEntity", "m_vecMaxs")
	NETVAR(m_flC4Blow, float, "DT_PlantedC4", "m_flC4Blow")
	OFFSET(m_VarMapping, VarMapping_t, 0x24)
	NETVAR(m_flSimulationTime, float, "DT_BaseEntity", "m_flSimulationTime")
	NETVAR_O(m_flOldSimulationTime, float, "DT_BaseEntity", "m_flSimulationTime", 0x4)

	CBaseEntity* m_pMoveParent();

	void* networkable( ) {
		return reinterpret_cast< void* >( uintptr_t( this ) + 0x8 );
	}


	inline IClientRenderable* GetClientRenderable() { return reinterpret_cast<IClientRenderable*>((uintptr_t)this + 0x4); };
	IClientNetworkable* GetClientNetworkable();
	IClientUnknown* GetClientUnknown();
	ICollideable* GetCollideable();

	bool IsPlayer();
	bool IsWeapon();
	ClientClass* GetClientClass();
	const model_t* GetModel();
	Vector GetAbsOrigin();
	QAngle GetAbsAngles();
	bool IsBreakable();
	datamap_t* GetPredDescMap();
	
	Vector GetWorldPosition();

	inline CBasePlayer* GetPlayer() {
		return (CBasePlayer*)this;
	}

	void SetModelIndex(int index) {
		CallVFunction<void(__thiscall*)(CBaseEntity*, int)>(this, 75)(this, index);
	}

	void SetAbsOrigin(Vector origin);
	void SetAbsAngles(QAngle angles);

	void SetCollisionBounds(Vector mins, Vector maxs);
};

class CBaseViewModel : public CBaseEntity {
public:
	NETVAR( m_nSequence, int, "DT_BaseViewModel", "m_nSequence" )
	NETVAR( m_nAnimationParity, int, "DT_BaseViewModel", "m_nAnimationParity" )
	NETVAR( m_nModelIndex, int, "DT_BaseViewModel", "m_nModelIndex" )
	NETVAR( m_hWeapon, int, "DT_BaseViewModel", "m_hWeapon" )
	NETVAR(m_hOwner, unsigned long, "DT_BaseViewModel", "m_hOwner")
};

class CEconViewItem : public CBaseEntity {
public:
	NETVAR( is_initialized, bool, "DT_ScriptCreatedItem", "m_bInitialized" )
	NETVAR( entity_level, int, "DT_ScriptCreatedItem", "m_iEntityLevel" )
	NETVAR( account_id, int, "DT_ScriptCreatedItem", "m_iAccountID" )
	NETVAR( item_id_low, int, "DT_ScriptCreatedItem", "m_iItemIDLow" )

};

struct item_schema_t {
	template < typename _key_t, typename _value_t >
	struct node_t {
		int			m_prev_id{};
		int			m_next_id{};
		char		pad0[8u]{};
		_key_t		m_key{};
		_value_t	m_value{};
	};

	template < typename _key_t, typename _value_t >
	struct head_t {
		node_t< _key_t, _value_t >* m_mem{};
		int							m_alloc_count{};
		int							m_grow_size{};
		int							m_start_element{};
		int							m_next_available{};
		char						pad0[4u]{};
		int							m_last_element{};
	};

	struct str_t {
		char* m_buffer{};
		int		m_capacity{};
		int		m_grow_size{};
		int		m_length{};
	};

	struct paint_kit_t {
		int		m_id{};
		str_t	m_name{};
		str_t	m_description{};
		str_t	m_item_name{};
	};

	char						pad0[652u]{};
	head_t< int, paint_kit_t* > m_paint_kits{};
};

class CAttributableItem : public CBaseEntity {
public:
	NETVAR(m_OriginalOwnerXuidLow, int, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow")
	NETVAR(m_OriginalOwnerXuidHigh, int, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh")
	NETVAR(m_nFallbackStatTrak, int, "DT_BaseAttributableItem", "m_nFallbackStatTrak")
	NETVAR(m_nFallbackPaintKit, int, "DT_BaseAttributableItem", "m_nFallbackPaintKit")
	NETVAR(m_nFallbackSeed, int, "DT_BaseAttributableItem", "m_nFallbackSeed")
	NETVAR(m_iAccountID, int, "DT_BaseAttributableItem", "m_iAccountID")
	NETVAR(m_iEntityQuality, int, "DT_BaseAttributableItem", "m_iEntityQuality")
	NETVAR(m_flFallbackWear, float, "DT_BaseAttributableItem", "m_flFallbackWear")
	NETVAR(m_hWeaponWorldModel, unsigned long, "DT_BaseCombatWeapon", "m_hWeaponWorldModel")
	NETVAR(m_iItemDefinitionIndex, short, "DT_BaseAttributableItem", "m_iItemDefinitionIndex")
	NETVAR(m_iItemIDHigh, int, "DT_BaseAttributableItem", "m_iItemIDHigh")
	NETVAR(m_zCustomName, char*, "DT_BaseAttributableItem", "m_szCustomName")

	void net_pre_data_update( int update_type ) {
		using original_fn = void( __thiscall* )( void*, int );
		return ( *( original_fn** )networkable( ) )[ 6 ]( networkable( ), update_type );
	}

	void net_release( ) {
		using original_fn = void( __thiscall* )( void* );
		return ( *( original_fn** )networkable( ) )[ 1 ]( networkable( ) );
	}

	int net_set_destroyed_on_recreate_entities( ) {
		using original_fn = int( __thiscall* )( void* );
		return ( *( original_fn** )networkable( ) )[ 13 ]( networkable( ) );
	}

	CBaseViewModel* get_view_model( ) {
		return ( CBaseViewModel* )this;
	}

	CEconViewItem* item( ) {
		return ( CEconViewItem* )this;
	}
};


class CBaseGrenade : public CBaseEntity {
public:
	OFFSET(m_flInfernoSpawnTime, float, 0x20)
	NETVAR(m_nSmokeEffectTickBegin, int, "DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin")
	NETVAR_O(GetCreationTime, float, "DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin", 0x10)
	NETVAR(m_hThrower, unsigned long, "DT_BaseGrenade", "m_hThrower")
	NETVAR(m_flThrowTime, float, "DT_BaseCSGrenade", "m_fThrowTime")
	NETVAR(m_bPinPulled, bool, "DT_BaseCSGrenade", "m_bPinPulled")
	NETVAR(m_bIsHeldByPlayer, bool, "DT_BaseCSGrenade", "m_bIsHeldByPlayer")
	NETVAR(m_flThrowStrength, float, "DT_BaseCSGrenade", "m_flThrowStrength")
	NETVAR(m_flNextPrimaryAttack, float, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack")

	CBasePlayer* GetThrower();
};

class CCascadeLight : public CBaseEntity {
public:
	NETVAR(m_shadowDirection, Vector, "DT_CascadeLight", "m_shadowDirection")
	NETVAR(m_envLightShadowDirection, Vector, "DT_CascadeLight", "m_envLightShadowDirection")
	NETVAR(m_flMaxShadowDist, float, "DT_CascadeLight", "m_flMaxShadowDist")
};

enum EWeaponType : int
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL = 1,
	WEAPONTYPE_SUBMACHINEGUN = 2,
	WEAPONTYPE_RIFLE = 3,
	WEAPONTYPE_SHOTGUN = 4,
	WEAPONTYPE_SNIPER = 5,
	WEAPONTYPE_MACHINEGUN = 6,
	WEAPONTYPE_C4 = 7,
	WEAPONTYPE_PLACEHOLDER = 8,
	WEAPONTYPE_GRENADE = 9,
	WEAPONTYPE_HEALTHSHOT = 11,
	WEAPONTYPE_FISTS = 12,
	WEAPONTYPE_BREACHCHARGE = 13,
	WEAPONTYPE_BUMPMINE = 14,
	WEAPONTYPE_TABLET = 15,
	WEAPONTYPE_MELEE = 16
};

enum EClassId {
	C_AI_BASE_NPC,
	C_WEAPON_AK47,
	C_BASE_ANIMATING,
	C_BASE_ANIMATING_OVERLAY,
	C_BASE_ATTRIBUTABLE_ITEM,
	C_BASE_BUTTON,
	C_BASE_COMBAT_CHARACTER,
	C_BASE_COMBAT_WEAPON,
	C_BASE_CS_GRENADE,
	C_BASE_CS_GRENADE_PROJECTILE,
	C_BASE_DOOR,
	C_BASE_ENTITY,
	C_BASE_FLEX,
	C_BASE_GRENADE,
	C_BASE_PARTICLE_ENTITY,
	C_BASE_PLAYER,
	C_BASE_PROP_DOOR,
	C_BASE_TEAM_OBJECTIVE_RESOURCE,
	C_BASE_TEMP_ENTITY,
	C_BASE_TOGGLE,
	C_BASE_TRIGGER,
	C_BASE_VIEWMODEL,
	C_BASE_VPHYSICS_TRIGGER,
	C_BASE_WEAPON_WORLD_MODEL,
	C_BEAM,
	C_BEAM_SPOTLIGHT,
	C_BONE_FOLLOWER,
	C_BR_C4_TARGET,
	C_BREACH_CHARGE,
	C_BREACH_CHARGE_PROJECTILE,
	C_BREAKABLE_PROP,
	C_BREAKABLE_SURFACE,
	C_BUMP_MINE,
	C_BUMP_MINE_PROJECTILE,
	C_C4,
	C_CASCADE_LIGHT,
	C_CHICKEN,
	C_COLOR_CORRECTION,
	C_COLOR_CORRECTION_VOLUME,
	C_CS_GAMERULES_PROXY,
	C_CS_PLAYER,
	C_CS_PLAYER_RESOURCE,
	C_CS_RAGDOLL,
	C_CS_TEAM,
	C_DANGERZONE,
	C_DANGERZONE_CONTROLLER,
	C_DEAGLE,
	C_DECOY_GRENADE,
	C_DECOY_PROJECTILE,
	C_DRONE,
	C_DRONE_GUN,
	C_DYNAMIC_LIGHT,
	C_DYNAMIC_PROP,
	C_ECON_ENTITY,
	C_ECON_WEARABLE,
	C_EMBERS,
	C_ENTITY_DISSOLVE,
	C_ENTITY_FLAME,
	C_ENTITY_FREEZING,
	C_ENTITY_PARTICLE_TRAIL,
	C_ENV_AMBIENT_LIGHT,
	C_ENV_DETAIL_CONTROLLER,
	C_ENV_DOF_CONTROLLER,
	C_ENV_GAS_CANISTER,
	C_ENV_PARTICLE_SCRIPT,
	C_ENV_PROJECTED_TEXTURE,
	C_ENV_QUADRATIC_BEAM,
	C_ENV_SCREEN_EFFECT,
	C_ENV_SCREEN_OVERLAY,
	C_ENV_TONEMAP_CONTROLLER,
	C_ENV_WIND,
	C_FE_PLAYER_DECAL,
	C_FIRECRACKER_BLAST,
	C_FIRE_SMOKE,
	C_FIRE_TRAIL,
	C_FISH,
	C_FISTS,
	C_FLASHBANG,
	C_FOG_CONTROLLER,
	C_FOOTSTEP_CONTROL,
	C_FUNC_DUST,
	C_FUNC_LOD,
	C_FUNC_AREA_PORTAL_WINDOW,
	C_FUNC_BRUSH,
	C_FUNC_CONVEYOR,
	C_FUNC_LADDER,
	C_FUNC_MONITOR,
	C_FUNC_MOVE_LINEAR,
	C_FUNC_OCCLUDER,
	C_FUNC_REFLECTIVE_GLASS,
	C_FUNC_ROTATING,
	C_FUNC_SMOKE_VOLUME,
	C_FUNC_TRACK_TRAIN,
	C_GAMERULES_PROXY,
	C_GRASS_BURN,
	C_HANDLE_TEST,
	C_HE_GRENADE,
	C_HOSTAGE,
	C_HOSTAGE_CARRIABLE_PROP,
	C_INCENDIARY_GRENADE,
	C_INFERNO,
	C_INFO_LADDER_DISMOUNT,
	C_INFO_MAP_REGION,
	C_INFO_OVERLAY_ACCESSOR,
	C_ITEM_HEALTHSHOT,
	C_ITEM_CASH,
	C_ITEM_DOGTAGS,
	C_KNIFE,
	C_KNIFE_GG,
	C_LIGHT_GLOW,
	C_MAP_VETO_PICK_CONTROLLER,
	C_MATERIAL_MODIFY_CONTROL,
	C_MELEE,
	C_MOLOTOV_GRENADE,
	C_MOLOTOV_PROJECTILE,
	C_MOVIE_DISPLAY,
	C_PARADROP_CHOPPER,
	C_PARTICLE_FIRE,
	C_PARTICLE_PERFORMANCE_MONITOR,
	C_PARTICLE_SYSTEM,
	C_PHYS_BOX,
	C_PHYS_BOX_MULTIPLAYER,
	C_PHYSICS_PROP,
	C_PHYSICS_PROP_MULTIPLAYER,
	C_PHYS_MAGNET,
	C_PHYS_PROP_AMMO_BOX,
	C_PHYS_PROP_LOOT_CRATE,
	C_PHYS_PROP_RADAR_JAMMER,
	C_PHYS_PROP_WEAPON_UPGRADE,
	C_PLANTED_C4,
	C_PLASMA,
	C_PLAYER_PING,
	C_PLAYER_RESOURCE,
	C_POINT_CAMERA,
	C_POINT_COMMENT_ARYNODE,
	C_POINT_WORLD_TEXT,
	C_POSE_CONTROLLER,
	C_POST_PROCESS_CONTROLLER,
	C_PRECIPITATION,
	C_PRECIPITATION_BLOCKER,
	C_PREDICTED_VIEW_MODEL,
	C_PROP_HALLUCINATION,
	C_PROP_COUNTER,
	C_PROP_DOOR_ROTATING,
	C_PROP_JEEP,
	C_PROP_VEHICLE_DRIVEABLE,
	C_RAGDOLL_MANAGER,
	C_RAGDOLL_PROP,
	C_RAGDOLL_PROP_ATTACHED,
	C_ROPE_KEYFRAME,
	C_WEAPON_SCAR17,
	C_SCENE_ENTITY,
	C_SENSOR_GRENADE,
	C_SENSOR_GRENADE_PROJECTILE,
	C_SHADOW_CONTROL,
	C_SLIDE_SHOW_DISPLAY,
	C_SMOKE_GRENADE,
	C_SMOKE_GRENADE_PROJECTILE,
	C_SMOKE_STACK,
	C_SNOWBALL,
	C_SNOWBALL_PILE,
	C_SNOWBALL_PROJECTILE,
	C_SPATIAL_ENTITY,
	C_SPOTLIGHT_END,
	C_SPRITE,
	C_SPRITE_ORIENTED,
	C_SPRITE_TRAIL,
	C_STATUE_PROP,
	C_STEAMJET,
	C_SUN,
	C_SUNLIGHT_SHADOW_CONTROL,
	C_SURVIVAL_SPAWN_CHOPPER,
	C_TABLET,
	C_TEAM,
	C_TEAM_PLAYROUND_BASED_RULES_PROXY,
	C_TEAR_MORRICOCHET,
	C_TE_BASE_BEAM,
	C_TE_BEAM_ENT_POINT,
	C_TE_BEAM_ENTS,
	C_TE_BEAM_FOLLOW,
	C_TE_BEAM_LASER,
	C_TE_BEAM_POINTS,
	C_TE_BEAM_RING,
	C_TE_BEAM_RING_POINT,
	C_TE_BEAM_SPLINE,
	C_TE_BLOOD_SPRITE,
	C_TE_BLOOD_STREAM,
	C_TE_BREAK_MODEL,
	C_TE_BSP_DECAL,
	C_TE_BUBBLES,
	C_TE_BUBBLE_TRAIL,
	C_TE_CLIENT_PROJECTILE,
	C_TE_DECAL,
	C_TE_DUST,
	C_TE_DYNAMIC_LIGHT,
	C_TE_EFFECT_DISPATCH,
	C_TE_ENERGY_SPLASH,
	C_TE_EXPLOSION,
	C_TE_FIRE_BULLETS,
	C_TE_FIZZ,
	C_TE_FOOTPRINT_DECAL,
	C_TE_FOUNDRY_HELPERS,
	C_TE_GAUSS_EXPLOSION,
	C_TE_GLOW_SPRITE,
	C_TE_IMPACT,
	C_TE_KILL_PLAYER_ATTACHMENTS,
	C_TE_LARGE_FUNNEL,
	C_TE_METAL_SPARKS,
	C_TE_MUZZLE_FLASH,
	C_TE_PARTICLE_SYSTEM,
	C_TE_PHYSICS_PROP,
	C_TE_PLANT_BOMB,
	C_TE_PLAYER_ANIM_EVENT,
	C_TE_PLAYER_DECAL,
	C_TE_PROJECTED_DECAL,
	C_TE_RADIOICON,
	C_TE_SHATTER_SURFACE,
	C_TE_SHOW_LINE,
	C_TE_SLA,
	C_TE_SMOKE,
	C_TE_SPARKS,
	C_TE_SPRITE,
	C_TE_SPRITE_SPRAY,
	C_TEST_PROXY_TOGGLE_NETWORKABLE,
	C_TEST_TRACE_LINE,
	C_TE_WORLD_DECAL,
	C_TRIGGER_PLAYER_MOVEMENT,
	C_TRIGGER_SOUND_OPERATOR,
	C_VGUI_SCREEN,
	C_VOTE_CONTROLLER,
	C_WATER_BULLET,
	C_WATER_LOD_CONTROL,
	C_WEAPON_AUG,
	C_WEAPON_AWP,
	C_WEAPON_BASE_ITEM,
	C_WEAPON_PPBIZON,
	C_WEAPON_CS_BASE,
	C_WEAPON_CS_BASE_GUN,
	C_WEAPON_CYCLER,
	C_WEAPON_DUAL_BERETTAS,
	C_WEAPON_FAMAS,
	C_WEAPON_FIVESEVEN,
	C_WEAPON_G3SG1,
	C_WEAPON_GALIL,
	C_WEAPON_GALIL_AR,
	C_WEAPON_GLOCK18,
	C_WEAPON_P2000,
	C_WEAPON_M249,
	C_WEAPON_M3,
	C_WEAPON_M4A4,
	C_WEAPON_MAC10,
	C_WEAPON_MAG7,
	C_WEAPON_MP5SD,
	C_WEAPON_MP7,
	C_WEAPON_MP9,
	C_WEAPON_NEGEV,
	C_WEAPON_NOVA,
	C_WEAPON_P228,
	C_WEAPON_P250,
	C_WEAPON_P90,
	C_WEAPON_SAWEDOFF,
	C_WEAPON_SCAR20,
	C_WEAPON_SCOUT,
	C_WEAPON_SG550,
	C_WEAPON_SG552,
	C_WEAPON_SG553,
	C_WEAPON_BALLISTIC_SHIELD,
	C_WEAPON_SSG08,
	C_WEAPON_ZEUSX27,
	C_WEAPON_TEC9,
	C_WEAPON_TMP,
	C_WEAPON_UMP45,
	C_WEAPON_USP,
	C_WEAPON_XM1014,
	C_WORLD,
	C_WORLD_VGUI_TEXT,
	DUST_TRAIL,
	MOVIE_EXPLOSION,
	PARTICLE_SMOKE_GRENADE,
	ROCKET_TRAIL,
	SMOKE_TRAIL,
	SPORE_EXPLOSION,
	SPORE_TRAIL
};
