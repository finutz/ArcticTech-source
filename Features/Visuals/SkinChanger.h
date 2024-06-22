#pragma once

#include "../../SDK/Interfaces/IBaseClientDLL.h"
#include "../../SDK/Globals.h"
#include "../../SDK/Config.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Misc/CBaseCombatWeapon.h"

#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1
#define RandomIntDef(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

class CBaseCombatWeapon;

struct KnifeModel_t {
	std::string ui_name;
	std::string model_name;
};

struct PaintKit
{
	int id;
	const wchar_t* name;
};

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;
struct CPaintKit;
struct Node_t;
struct Head_t;

struct Head_t
{
	Node_t* pMemory;		//0x0000
	int nAllocationCount;	//0x0004
	int nGrowSize;			//0x0008
	int nStartElement;		//0x000C
	int nNextAvailable;		//0x0010
	int _unknown;			//0x0014
	int nLastElement;		//0x0018
}; //Size=0x001C

struct Node_t
{
	int nPreviousId;		//0x0000
	int nNextId;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	int iPaintKitId;		//0x0010
	CPaintKit* pPaintKit;	//0x0014
}; //Size=0x0018

struct String_t
{
	char* szBuffer;	//0x0000 
	int nCapacity;	//0x0004 
	int _unknown;	//0x0008 
	int nLength;	//0x000C 
}; //Size=0x0010

struct CPaintKit
{
	int iIndex;				//0x0000
	String_t Name;			//0x0004
	String_t NiceName;		//0x0014
	String_t Tag;			//0x0024
	String_t ShortName;		//0x0034
	String_t _unknown;		//0x0044
	char pad_0x0054[0x8C];	//0x0054
}; //Size=0x00E0


class CSkinChanger {
	std::vector<KnifeModel_t> knife_models;
	RecvVarProxy_t fnSequenceProxyFn = nullptr;
	RecvVarProxy_t oRecvnModelIndex;
public:
	void LoadKnifeModels();
	void InitPaintkit();
	std::vector<PaintKit> vecKits;
	std::vector<PaintKit> vecKits_gloves;
	std::vector<PaintKit> GetPaintKits();
	std::vector<PaintKit> GetGlovePaintKits();
	std::vector<std::string> GetUIPaintKits();
	std::vector<std::string> GetUIPaintKitsGloves();
	std::vector<std::string> GetUIKnifeModels();
	bool ApplyKnifeModel( CAttributableItem* weapon, const char* model );
	static void SetViewModelSequence( const CRecvProxyData* pDataConst, void* pStruct, void* pOut );
	static void Hooked_RecvProxy_Viewmodel( CRecvProxyData* pData, void* pStruct, void* pOut );
	void FixViewModelSequence();
	void AnimationUnHook( );
	const char* default_mask = "models/player/holiday/facemasks/facemask_battlemask.mdl";

	bool LoadModel( const char* thisModelName );
	void InitCustomModels();
	void UpdateSkins();
	void MaskChanger();
	typedef void(__thiscall* UpdateAddonModelsFunc)(void*, bool);

	void AgentChanger( );
	static constexpr std::array models_ct {
		"models/player/custom_player/legacy/tm_phoenix.mdl",
			"models/player/custom_player/legacy/ctm_sas.mdl",
			"models/player/custom_player/legacy/tm_professional_varf1.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variante.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variantb2.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variantb.mdl",
			"models/player/custom_player/legacy/tm_professional_varf2.mdl",
			"models/player/custom_player/legacy/ctm_gendarmerie_variantc.mdl",
			"models/player/custom_player/legacy/ctm_diver_variantb.mdl",
			"models/player/custom_player/legacy/ctm_diver_varianta.mdl",
			"models/player/custom_player/legacy/tm_professional_varf3.mdl",
			"models/player/custom_player/legacy/tm_professional_varf4.mdl",
			"models/player/custom_player/legacy/tm_professional_varf.mdl",
			"models/player/custom_player/legacy/tm_professional_varj.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_varianta.mdl",
			"models/player/custom_player/legacy/tm_professional_varf5.mdl",
			"models/player/custom_player/legacy/ctm_gendarmerie_variantb.mdl",
			"models/player/custom_player/legacy/ctm_diver_variantc.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variant s.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variantd.mdl",
			"models/player/custom_player/legacy/ctm_gendarmerie_variante.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variantf2.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantk.mdl",
			"models/player/custom_player/legacy/ctm_gendarmerie_varianta.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantn.mdl",
			"models/player/custom_player/legacy/ctm_sas_variantg.mdl",
			"models/player/custom_player/legacy/tm_jungle_raider_variantf.mdl",
			"models/player/custom_player/legacy/ctm_gendarmerie_variantd.mdl",
			"models/player/custom_player/legacy/tm_professional_vari.mdl",
			"models/player/custom_player/legacy/tm_professional_varg.mdl",
			"models/player/custom_player/legacy/tm_professional_varh.mdl",
			"models/player/custom_player/legacy/tm_balkan_variantj.mdl",
			"models/player/custom_player/legacy/tm_balkan_variantk.mdl",
			"models/player/custom_player/legacy/tm_balkan_variantg.mdl",
			"models/player/custom_player/legacy/tm_balkan_varianti.mdl",
			"models/player/custom_player/legacy/tm_balkan_variantf.mdl",
			"models/player/custom_player/legacy/tm_balkan_variantl.mdl",
			"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantm.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantl.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantj.mdl",
			"models/player/custom_player/legacy/ctm_st6_variante.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
			"models/player/custom_player/legacy/tm_balkan_varianth.mdl",
			"models/player/custom_player/legacy/ctm_fbi_varianth.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
			"models/player/custom_player/legacy/ctm_swat_variante.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantf.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantg.mdl",
			"models/player/custom_player/legacy/ctm_swat_varianth.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantj.mdl",
			"models/player/custom_player/legacy/ctm_swat_varianti.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
			"models/player/custom_player/legacy/tm_phoenix_varianti.mdl",
			"models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
			"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
			"models/player/custom_player/legacy/tm_phoenix_varianth.mdl",
			"models/player/custom_player/legacy/tm_leet_variantf.mdl",
			"models/player/custom_player/legacy/tm_leet_varianti.mdl",
			"models/player/custom_player/legacy/tm_leet_varianth.mdl",
			"models/player/custom_player/legacy/tm_leet_variantg.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
			"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
			"models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl",
			"models/player/custom_player/legacy/tm_jumpsuit_variantb.mdl",
			"models/player/custom_player/legacy/tm_jumpsuit_variantc.mdl",
			"models/player/custom_player/legacy/tm_anarchist_varianta.mdl",
			"models/player/custom_player/legacy/tm_anarchist_variantb.mdl",
			"models/player/custom_player/legacy/tm_anarchist_variantc.mdl",
			"models/player/custom_player/legacy/tm_separatist_varianta.mdl",
			"models/player/custom_player/legacy/tm_separatist_variantb.mdl",
			"models/player/custom_player/legacy/tm_separatist_variantc.mdl",
			"models/player/custom_player/legacy/tm_separatist_variantd.mdl",
			"models/player/custom_player/legacy/ctm_fbi.mdl",
			"models/player/custom_player/legacy/ctm_fbi_varianta.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variantc.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variantd.mdl",
			"models/player/custom_player/legacy/ctm_fbi_variante.mdl",
			"models/player/custom_player/legacy/ctm_gign_varianta.mdl",
			"models/player/custom_player/legacy/ctm_gign_variantb.mdl",
			"models/player/custom_player/legacy/ctm_gign_variantc.mdl",
			"models/player/custom_player/legacy/ctm_st6.mdl",
			"models/player/custom_player/legacy/ctm_st6_varianta.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantb.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantc.mdl",
			"models/player/custom_player/legacy/ctm_st6_variantd.mdl",
			"models/player/custom_player/legacy/ctm_idf_variantb.mdl",
			"models/player/custom_player/legacy/ctm_idf_variantc.mdl",
			"models/player/custom_player/legacy/ctm_idf_variantd.mdl",
			"models/player/custom_player/legacy/ctm_idf_variante.mdl",
			"models/player/custom_player/legacy/ctm_idf_variantf.mdl",
			"models/player/custom_player/legacy/ctm_swat.mdl",
			"models/player/custom_player/legacy/ctm_swat_varianta.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantb.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantc.mdl",
			"models/player/custom_player/legacy/ctm_swat_variantd.mdl",
			"models/player/custom_player/legacy/ctm_sas.mdl",
			"models/player/custom_player/legacy/ctm_gsg9.mdl",
			"models/player/custom_player/legacy/ctm_gsg9_varianta.mdl",
			"models/player/custom_player/legacy/ctm_gsg9_variantb.mdl",
			"models/player/custom_player/legacy/ctm_gsg9_variantc.mdl",
			"models/player/custom_player/legacy/ctm_gsg9_variantd.mdl",
			"models/player/custom_player/legacy/tm_professional_var1.mdl",
			"models/player/custom_player/legacy/tm_professional_var2.mdl",
			"models/player/custom_player/legacy/tm_professional_var3.mdl",
			"models/player/custom_player/legacy/tm_professional_var4.mdl",
			"models/player/custom_player/legacy/tm_leet_varianta.mdl",
			"models/player/custom_player/legacy/tm_leet_variantb.mdl",
			"models/player/custom_player/legacy/tm_leet_variantc.mdl",
			"models/player/custom_player/legacy/tm_leet_variantd.mdl",
			"models/player/custom_player/legacy/tm_Balkan_varianta.mdl",
			"models/player/custom_player/legacy/tm_Balkan_variantb.mdl",
			"models/player/custom_player/legacy/tm_Balkan_variantc.mdl",
			"models/player/custom_player/legacy/tm_Balkan_variantd.mdl",
			"models/player/custom_player/legacy/tm_pirate_varianta.mdl",
			"models/player/custom_player/legacy/tm_pirate_variantb.mdl",
			"models/player/custom_player/legacy/tm_pirate_variantc.mdl",
	};

	static constexpr std::array mask_models
	{	"",
		"models/player/holiday/facemasks/facemask_dallas.mdl",
		"models/player/holiday/facemasks/facemask_battlemask.mdl",
		"models/player/holiday/facemasks/evil_clown.mdl",
		"models/player/holiday/facemasks/facemask_anaglyph.mdl",
		"models/player/holiday/facemasks/facemask_boar.mdl",
		"models/player/holiday/facemasks/facemask_bunny.mdl",
		"models/player/holiday/facemasks/facemask_bunny_gold.mdl",
		"models/player/holiday/facemasks/facemask_chains.mdl",
		"models/player/holiday/facemasks/facemask_chicken.mdl",
		"models/player/holiday/facemasks/facemask_devil_plastic.mdl",
		"models/player/holiday/facemasks/facemask_hoxton.mdl",
		"models/player/holiday/facemasks/facemask_pumpkin.mdl",
		"models/player/holiday/facemasks/facemask_samurai.mdl",
		"models/player/holiday/facemasks/facemask_sheep_bloody.mdl",
		"models/player/holiday/facemasks/facemask_sheep_gold.mdl",
		"models/player/holiday/facemasks/facemask_sheep_model.mdl",
		"models/player/holiday/facemasks/facemask_skull.mdl",
		"models/player/holiday/facemasks/facemask_template.mdl",
		"models/player/holiday/facemasks/facemask_wolf.mdl",
		"models/player/holiday/facemasks/porcelain_doll.mdl",
	};


	void Run( bool frame_end );
};

extern CSkinChanger* SkinChanger;