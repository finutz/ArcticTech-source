#include "Interfaces.h"

#include "../Utils/Utils.h"
#include "Globals.h"

IClientEntityList* EntityList;
IVEngineClient* EngineClient;
IBaseClientDLL* Client;
IClientMode* ClientMode;
IDirect3DDevice9* DirectXDevice;
ICvar* CVar;
IPrediction* Prediction;
CGlobalVarsBase* GlobalVars;
CClientState* ClientState;
ISurface* Surface;
IInputSystem* InputSystem;
IPanel* VPanel;
CCSPlayerResource* PlayerResource;
IWeaponSystem* WeaponSystem;
ILocalize* Localize;
CGlowObjectManager* GlowObjectManager;
IMaterialSystem* MaterialSystem;
IVModelInfoClient* ModelInfoClient;
IVModelRender* ModelRender;
IStudioRender* StudioRender;
IVRenderView* RenderView;
CInput* Input;
IEngineTrace* EngineTrace;
IGameMovement* GameMovement;
IMoveHelper* MoveHelper;
IPhysicsSurfaceProps* PhysicSurfaceProps;
IGameEventManager2* GameEventManager;
IViewRenderBeams* ViewRenderBeams;
IVDebugOverlay* DebugOverlay;
IMDLCache* MDLCache;
IKeyValuesSystem* KeyValuesSystem;
IEngineSound* EngineSound;
IEffects* Effects;
INetworkStringTableContainer* NetworkStringTableContainer;
ISteamHTTP* SteamHTTP;
CStaticPropMgr* StaticPropMgr;

void Interfaces::Initialize() {
	EntityList = (IClientEntityList*)Utils::CreateInterface("client.dll", "VClientEntityList003");
	EngineClient = (IVEngineClient*)Utils::CreateInterface("engine.dll", "VEngineClient014");
	Client = (IBaseClientDLL*)Utils::CreateInterface("client.dll", "VClient018");
	CVar = (ICvar*)Utils::CreateInterface("vstdlib.dll", "VEngineCvar007");
	Prediction = (IPrediction*)Utils::CreateInterface("client.dll", "VClientPrediction001");
	Surface = (ISurface*)Utils::CreateInterface("vguimatsurface.dll", "VGUI_Surface031");
	InputSystem = (IInputSystem*)Utils::CreateInterface("inputsystem.dll", "InputSystemVersion001");
	VPanel = (IPanel*)Utils::CreateInterface("vgui2.dll", "VGUI_Panel009");
	Localize = (ILocalize*)Utils::CreateInterface("localize.dll", "Localize_001");
	MaterialSystem = (IMaterialSystem*)Utils::CreateInterface("materialsystem.dll", "VMaterialSystem080");
	ModelRender = (IVModelRender*)Utils::CreateInterface("engine.dll", "VEngineModel016");
	StudioRender = (IStudioRender*)Utils::CreateInterface("studiorender.dll", "VStudioRender026");
	ModelInfoClient = (IVModelInfoClient*)Utils::CreateInterface("engine.dll", "VModelInfoClient004");
	RenderView = (IVRenderView*)Utils::CreateInterface("engine.dll", "VEngineRenderView014");
	EngineTrace = (IEngineTrace*)Utils::CreateInterface("engine.dll", "EngineTraceClient004");
	GameMovement = (IGameMovement*)Utils::CreateInterface("client.dll", "GameMovement001");
	GameEventManager = (IGameEventManager2*)Utils::CreateInterface("engine.dll", "GAMEEVENTSMANAGER002");
	PhysicSurfaceProps = (IPhysicsSurfaceProps*)(Utils::CreateInterface("vphysics.dll", "VPhysicsSurfaceProps001"));
	DebugOverlay = (IVDebugOverlay*)Utils::CreateInterface("engine.dll", "VDebugOverlay004");
	MDLCache = (IMDLCache*)Utils::CreateInterface("datacache.dll", "MDLCache004");
	EngineSound = (IEngineSound*)Utils::CreateInterface("engine.dll", "IEngineSoundClient003");
	Effects = (IEffects*)Utils::CreateInterface("client.dll", "IEffects001");
	NetworkStringTableContainer = (INetworkStringTableContainer*)Utils::CreateInterface("engine.dll", "VEngineClientStringTable001");
	StaticPropMgr = (CStaticPropMgr*)Utils::CreateInterface("engine.dll", "StaticPropMgrClient005");
	GlobalVars = **(CGlobalVarsBase***)(Utils::PatternScan("client.dll", "A1 ? ? ? ? 5E 8B 40 10", 0x1));
	DirectXDevice = **(IDirect3DDevice9***)(Utils::PatternScan("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C", 0x1));
	PlayerResource = **(CCSPlayerResource***)(Utils::PatternScan("client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7", 0x2));
	WeaponSystem = *(IWeaponSystem**)(Utils::PatternScan("client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0", 0x2));
	GlowObjectManager = *(CGlowObjectManager**)(Utils::PatternScan("client.dll", "0F 11 05 ? ? ? ? 83 C8 01", 0x3));
	ViewRenderBeams = *(IViewRenderBeams**)(Utils::PatternScan("client.dll", "B9 ? ? ? ? 50 A1 ? ? ? ? FF 50 14", 0x1));
	SteamHTTP = (ISteamHTTP*)(reinterpret_cast<uint32_t**>(reinterpret_cast<char**>(reinterpret_cast<char*>(Utils::PatternScan("client.dll", "B9 ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 0F 84")) + 1)[0] + 48)[0]);

	KeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(Utils::GetExportAddress(Utils::GetModuleBaseHandle("vstdlib.dll"), "KeyValuesSystem"))();

	ClientMode = **(IClientMode***)((*(uintptr_t**)Client)[10] + 0x5);
	ClientState = **(CClientState***)((*(uintptr_t**)EngineClient)[12] + 0x10);
	Input = *(CInput**)((*(uintptr_t**)Client)[16] + 1);

	cvars.r_aspectratio = CVar->FindVar("r_aspectratio");
	cvars.mat_postprocessing_enable = CVar->FindVar("mat_postprocess_enable");
	cvars.r_DrawSpecificStaticProp = CVar->FindVar("r_DrawSpecificStaticProp");
	cvars.fog_override = CVar->FindVar("fog_override");
	cvars.fog_color = CVar->FindVar("fog_color");
	cvars.post_processing = CVar->FindVar( "fog_color" );
	cvars.fog_start = CVar->FindVar("fog_start");
	cvars.fog_end = CVar->FindVar("fog_end");
	cvars.fog_maxdensity = CVar->FindVar("fog_maxdensity");
	cvars.sv_cheats = CVar->FindVar("sv_cheats");
	cvars.molotov_throw_detonate_time = CVar->FindVar("molotov_throw_detonate_time");
	cvars.sv_gravity = CVar->FindVar("sv_gravity");
	cvars.weapon_recoil_scale = CVar->FindVar("weapon_recoil_scale");
	cvars.sv_jump_impulse = CVar->FindVar("sv_jump_impulse");
	cvars.sv_maxunlag = CVar->FindVar("sv_maxunlag");
	cvars.cl_csm_shadows = CVar->FindVar("cl_csm_shadows");
	cvars.cl_foot_contact_shadows = CVar->FindVar("cl_foot_contact_shadows");
	cvars.cl_lagcompensation = CVar->FindVar("cl_lagcompensation");
	cvars.cl_interp_ratio = CVar->FindVar("cl_interp_ratio");
	cvars.weapon_debug_spread_show = CVar->FindVar("weapon_debug_spread_show");
	cvars.r_drawsprites = CVar->FindVar("r_drawsprites");
	cvars.zoom_sensitivity_ratio_mouse = CVar->FindVar("zoom_sensitivity_ratio_mouse");
	cvars.mp_damage_headshot_only = CVar->FindVar("mp_damage_headshot_only");
}