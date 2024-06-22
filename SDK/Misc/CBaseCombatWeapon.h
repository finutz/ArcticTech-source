#pragma once

#include "CBaseEntity.h"

#include <string>

class CCSWeaponData
{
public:
	std::byte pad0[0x14];			// 0x0000
	int iMaxClip1;					// 0x0014
	int iMaxClip2;					// 0x0018
	int iDefaultClip1;				// 0x001C
	int iDefaultClip2;				// 0x0020
	int iPrimaryMaxReserveAmmo;		// 0x0024
	int iSecondaryMaxReserveAmmo;	// 0x0028
	const char* szWorldModel;		// 0x002C
	const char* szViewModel;		// 0x0030
	const char* szDroppedModel;		// 0x0034
	std::byte pad1[0x50];			// 0x0038
	const char* szHudName;			// 0x0088
	const char* szWeaponName;		// 0x008C
	std::byte pad2[0x2];			// 0x0090
	bool bIsMeleeWeapon;			// 0x0092
	std::byte pad3[0x9];			// 0x0093
	float flWeaponWeight;			// 0x009C
	std::byte pad4[0x4];			// 0x00A0
	int iSlot;						// 0x00A4
	int iPosition;					// 0x00A8
	std::byte pad5[0x1C];			// 0x00AC
	int nWeaponType;				// 0x00C8
	std::byte pad6[0x4];			// 0x00CC
	int iWeaponPrice;				// 0x00D0
	int iKillAward;					// 0x00D4
	const char* szAnimationPrefix;	// 0x00D8
	float flCycleTime;				// 0x00DC
	float flCycleTimeAlt;			// 0x00E0
	std::byte pad8[0x8];			// 0x00E4
	bool bFullAuto;					// 0x00EC
	std::byte pad9[0x3];			// 0x00ED
	int iDamage;					// 0x00F0
	float flHeadShotMultiplier;		// 0x00F4
	float flArmorRatio;				// 0x00F8
	int iBullets;					// 0x00FC
	float flPenetration;			// 0x0100
	std::byte pad10[0x8];			// 0x0104
	float flRange;					// 0x010C
	float flRangeModifier;			// 0x0110
	float flThrowVelocity;			// 0x0114
	std::byte pad11[0xC];			// 0x0118
	bool bHasSilencer;				// 0x0124
	std::byte pad12[0xF];			// 0x0125
	float flMaxSpeed;			    // 0x0134
    float flMaxSpeedAlt;
	std::byte pad13[0x4];			// 0x013C
	float flSpread[2];				// 0x0140
	float flInaccuracyCrouch[2];	// 0x0148
	float flInaccuracyStand[2];		// 0x0150
    float _flInaccuracyUnknown;     // 0x0158
    float flInaccuracyJumpInitial;// 0x015C
	float flInaccuracyJump[2];		// 0x0160
	float flInaccuracyLand[2];		// 0x0168
	float flInaccuracyLadder[2];	// 0x0170
	float flInaccuracyFire[2];		// 0x0178
	float flInaccuracyMove[2];		// 0x0180
	float flInaccuracyReload;		// 0x0188
	int iRecoilSeed;				// 0x018C
	float flRecoilAngle[2];			// 0x0190
	float flRecoilAngleVariance[2];	// 0x0198
	float flRecoilMagnitude[2];		// 0x01A0
	float flRecoilMagnitudeVariance[2]; // 0x01A8
	int iSpreadSeed;				// 0x01B0

    std::string GetName() {
        const wchar_t* name = Localize->FindSafe(szHudName);
        return Localize->utf16le_to_utf8(name);
    }
};

enum EItemDefinitionIndex
{
	//weapons
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE,// = 2, //Dual Berettas
	WEAPON_FIVESEVEN,// = 3,
	WEAPON_GLOCK,// = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG,// = 8,
	WEAPON_AWP,// = 9,
	WEAPON_FAMAS,// = 10,
	WEAPON_G3SG1,// = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249,// = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,// = 17,
	WEAPON_P90 = 19,
	WEAPON_ZONE_REPULSOR,// = 20, //co-op mission thingy
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,// = 24,
	WEAPON_XM1014,// = 25,
	WEAPON_BIZON,// = 26,
	WEAPON_MAG7,// = 27,
	WEAPON_NEGEV,// = 28,
	WEAPON_SAWEDOFF,// = 29,
	WEAPON_TEC9,// = 30,
	WEAPON_TASER,// = 31,
	WEAPON_HKP2000,// = 32, //P2000
	WEAPON_MP7,// = 33,
	WEAPON_MP9,// = 34,
	WEAPON_NOVA,// = 35,
	WEAPON_P250,// = 36,
	WEAPON_SHIELD,// = 37, //ballistic shield
	WEAPON_SCAR20,// = 38,
	WEAPON_SG556,// = 39, //SG 553
	WEAPON_SSG08,// = 40,
	WEAPON_KNIFEGG,// = 41, //Gold Knife
	WEAPON_KNIFE,// = 42, //Default (CT) Knife
	WEAPON_FLASHBANG,// = 43,
	WEAPON_HEGRENADE,// = 44,
	WEAPON_SMOKEGRENADE,// = 45,
	WEAPON_MOLOTOV,// = 46,
	WEAPON_DECOY,// = 47,
	WEAPON_INCGRENADE,// = 48,
	WEAPON_C4,// = 49,
	ITEM_KEVLAR,// = 50, //body armour
	ITEM_ASSAULTSUIT,// = 51, //helmet + body armour
	ITEM_HEAVYASSAULTSUIT,// = 52, //special armour in co-op modes
	ITEM_NVG = 54, //night vision goggles
	ITEM_DEFUSER,// = 55, //bomb defuser
	ITEM_CUTTERS,// = 56, //hostage cutter
	WEAPON_HEALTHSHOT,// = 57,
	WEAPON_KNIFE_T = 59, //Default (T) Knife
	WEAPON_M4A1_SILENCER,// = 60,
	WEAPON_USP_SILENCER,// = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,// = 64,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,// = 69,
	WEAPON_BREACHCHARGE,// = 70,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,// = 75,
	WEAPON_HAMMER,// = 76,
	WEAPON_SPANNER = 78, //Wrench
	WEAPON_KNIFE_GHOST = 80, //Spectral Shiv
	WEAPON_FIREBOMB,// = 81,
	WEAPON_DIVERSION,// = 82,
	WEAPON_FRAG_GRENADE,// = 83,
	WEAPON_SNOWBALL,// = 84,
	WEAPON_BUMPMINE,// = 85,
	WEAPON_BAYONET = 500, //Bayonet
	WEAPON_KNIFE_CSS = 503, //CSS Knife
	WEAPON_KNIFE_FLIP = 505, //Flip Knife
	WEAPON_KNIFE_GUT,// = 506, //Gut Knife
	WEAPON_KNIFE_KARAMBIT,// = 507, //Karambit
	WEAPON_KNIFE_M9_BAYONET,// = 508, //M9 Bayonet
	WEAPON_KNIFE_TACTICAL,// = 509, //Huntsman Knife
	WEAPON_KNIFE_FALCHION = 512, //Falchion Knife
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514, //Bowie Knife
	WEAPON_KNIFE_BUTTERFLY,// = 515, //Butterfly Knife
	WEAPON_KNIFE_PUSH,// = 516 //Shadow Daggers
	WEAPON_KNIFE_CORD,// = 517, //Paracord Knife
	WEAPON_KNIFE_CANIS,// = 518, //Survival Knife
	WEAPON_KNIFE_URSUS,// = 519, //Ursus Knife
	WEAPON_KNIFE_GYPSY_JACKKNIFE,// = 520 //Navaja Knife
	WEAPON_KNIFE_OUTDOOR,// = 521, //Nomad Knife
	WEAPON_KNIFE_STILETTO,// = 522, //Stiletto Knife
	WEAPON_KNIFE_WIDOWMAKER,// = 523, //Talon Knife
	WEAPON_KNIFE_SKELETON = 525, //Skeleton Knife
	STUDDED_BROKENFANG_GLOVES = 4725, //Brokenfang Gloves
	STUDDED_BLOODHOUND_GLOVES = 5027, //Bloodhound Gloves
	T_GLOVES,// = 5028, //Default (T) Gloves
	CT_GLOVES,// = 5029, //Default (CT) Gloves
	SPORTY_GLOVES,// = 5030, //Sporty Gloves
	SLICK_GLOVES,// = 5031, //Slick Gloves
	LEATHER_HANDWRAPS,// = 5032, //Hand Wraps
	MOTORCYCLE_GLOVES,// = 5033, //Moto Gloves
	SPECIALIST_GLOVES,// = 5034, //Specalist Gloves
	STUDDED_HYDRA_GLOVES,// = 5035, //Hydra Gloves
};

class CBaseCombatWeapon : public CBaseEntity {
public:
	NETVAR(m_flNextPrimaryAttack, float, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack")
    NETVAR(m_flNextSecondaryAttack, float, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack")
    NETVAR(m_flRecoilIndex, float, "DT_WeaponCSBase", "m_flRecoilIndex")
	OFFSET(m_iItemDefinitionIndex, short, 0x2FBA)
    NETVAR(m_hOwner, unsigned long, "DT_BaseCombatWeapon", "m_hOwner")
    NETVAR(m_iClip, unsigned long, "DT_BaseCombatWeapon", "m_iClip1")
    NETVAR(m_fAccuracyPenalty, float, "CWeaponCSBase", "m_fAccuracyPenalty")
    NETVAR(m_hWeaponWorldModel, unsigned long, "DT_BaseCombatWeapon", "m_hWeaponWorldModel")
    NETVAR(m_fLastShotTime, float, "DT_WeaponCSBase", "m_fLastShotTime")
    NETVAR(m_flPostponeFireReadyTime, float, "DT_WeaponCSBase", "m_flPostponeFireReadyTime")
	NETVAR(m_zoomLevel, int, "DT_WeaponCSBaseGun", "m_zoomLevel")

    inline bool IsGrenade() {
        if (!this)
            return false;

        return GetWeaponInfo()->nWeaponType == WEAPONTYPE_GRENADE;
    }

    inline void UpdateAccuracyPenality() {
        CallVFunction<void(__thiscall*)(CBaseCombatWeapon*)>(this, 484)(this);
    }

    inline float GetInaccuracy() {
		return CallVFunction<float(__thiscall*)(CBaseCombatWeapon*)>(this, 483)(this);
    }

    inline float GetSpread() {
        return CallVFunction<float(__thiscall*)(CBaseCombatWeapon*)>(this, 453)(this);
    }

    inline float MaxSpeed() {
        return CallVFunction<float(__thiscall*)(CBaseCombatWeapon*)>(this, 442)(this);
    }

    inline bool ShootingWeapon() {
        if (!this)
            return false;

        int weapon_type = GetWeaponInfo()->nWeaponType;
        return weapon_type == WEAPONTYPE_PISTOL || weapon_type == WEAPONTYPE_SHOTGUN || weapon_type == WEAPONTYPE_SNIPER || weapon_type == WEAPONTYPE_RIFLE;
    }

	CCSWeaponData* GetWeaponInfo();
	std::string GetName(CCSWeaponData* custom_data = nullptr);
    bool CanShoot(bool revolver_check = true);
	DXImage& GetIcon();
};

enum WeaponId : short {
    None = 0,
    Deagle = 1,
    Elite,
    Fiveseven,
    Glock,
    Ak47 = 7,
    Aug,
    Awp,
    Famas,
    G3SG1,
    GalilAr = 13,
    M249,
    M4A1 = 16,
    Mac10,
    P90 = 19,
    ZoneRepulsor,
    Mp5sd = 23,
    Ump45,
    Xm1014,
    Bizon,
    Mag7,
    Negev,
    Sawedoff,
    Tec9,
    Taser,
    Hkp2000,
    Mp7,
    Mp9,
    Nova,
    P250,
    Shield,
    Scar20,
    Sg553,
    Ssg08,
    GoldenKnife,
    Knife,
    Flashbang = 43,
    HeGrenade,
    SmokeGrenade,
    Molotov,
    Decoy,
    IncGrenade,
    C4,
    Healthshot = 57,
    KnifeT = 59,
    M4a1_s,
    Usp_s,
    Cz75a = 63,
    Revolver,
    TaGrenade = 68,
    Axe = 75,
    Hammer,
    Spanner = 78,
    GhostKnife = 80,
    Firebomb,
    Diversion,
    FragGrenade,
    Snowball,
    BumpMine,
    Bayonet = 500,
    ClassicKnife = 503,
    Flip = 505,
    Gut,
    Karambit,
    M9Bayonet,
    Huntsman,
    Falchion = 512,
    Bowie = 514,
    Butterfly,
    Daggers,
    Paracord,
    SurvivalKnife,
    Ursus = 519,
    Navaja,
    NomadKnife,
    Stiletto = 522,
    Talon,
    SkeletonKnife = 525,
    NameTag = 1200,
    Sticker = 1209,
    MusicKit = 1314,
    SealedGraffiti = 1348,
    Graffiti = 1349,
    OperationHydraPass = 1352,
    BronzeOperationHydraCoin = 4353,
    Patch = 4609,
    Berlin2019SouvenirToken = 4628,
    GloveStuddedBrokenfang = 4725,
    Stockholm2021SouvenirToken = 4802,
    GloveStuddedBloodhound = 5027,
    GloveT,
    GloveCT,
    GloveSporty,
    GloveSlick,
    GloveLeatherWrap,
    GloveMotorcycle,
    GloveSpecialist,
    GloveHydra
};