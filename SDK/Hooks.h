#pragma once

#include "../SDK/Interfaces.h"
#include "../SDK/NetMessages.h"
#include "../Utils/VMTHook.h"
#include "../detours.h"

class CBasePlayer;
class CBaseCombatWeapon;

typedef HRESULT(__stdcall* tEndScene)(IDirect3DDevice9*);
typedef HRESULT(__stdcall* tReset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(__stdcall* tPresent)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef void(__fastcall* tLockCursor)(ISurface*, void*);
typedef void(__fastcall* tOverrideView)(IClientMode*, void*, CViewSetup*);
typedef void(__fastcall* tPaintTraverse)(IPanel*, void*, unsigned int, bool, bool);
typedef void(__fastcall* tDoPostScreenEffects)(IClientMode*, void*, CViewSetup*);
typedef bool(__fastcall* tIsPaused)(IVEngineClient*, void*);
typedef void(__thiscall* tDrawModelExecute)(IVModelRender*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
typedef void(__fastcall* tFrameStageNotify)(IBaseClientDLL*, void*, EClientFrameStage);
typedef void(__fastcall* tRunCommand)(IPrediction*, void*, CBasePlayer*, CUserCmd*, IMoveHelper*);
typedef void(__fastcall* tUpdateClientSideAnimation)(CBasePlayer*, void*);
typedef bool(__fastcall* tShouldSkipAnimationFrame)(void*, void*);
typedef bool(__fastcall* tShouldInterpolate)(CBasePlayer*, void*);
typedef void(__fastcall* tDoExtraBoneProcessing)(CBaseEntity*, void*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
typedef bool(__fastcall* tIsHLTV)(IVEngineClient*, void*);
typedef void(__fastcall* tBuildTransformations)(CBaseEntity*, void*, void*, void*, void*, const void*, int, void*);
typedef bool(__fastcall* tSetupBones)(CBaseEntity*, void*, matrix3x4_t*, int, int, float);
typedef void(__cdecl* tCL_Move)(float, bool);
typedef void(__fastcall* tHudUpdate)(IBaseClientDLL*, void*, bool);
typedef void(__thiscall* tCHLCCreateMove)(IBaseClientDLL*, int, float, bool);
typedef MDLHandle_t(__fastcall* tFindMdlHook)(void* ecx, void* edx, char* FilePath);
typedef void(__fastcall* tPhysicsSimulate)(CBasePlayer*, void*);
typedef void(__fastcall* tClampBonesInBBox)(CBasePlayer*, void*, matrix3x4_t*, int);
typedef void(__fastcall* tCalculateView)(CBasePlayer*, void*, Vector&, QAngle&, float&, float&, float&);
typedef QAngle*(__fastcall* tGetEyeAngles)(CBasePlayer*, void*);
typedef void(__fastcall* tRenderSmokeOverlay)(void*, void*, bool);
typedef void(__fastcall* tPacketEnd)(CClientState*, void*);
typedef void(__fastcall* tPacketStart)(CClientState*, void*, int, int);
typedef bool(__fastcall* tFireGameEvent)(IGameEventManager2*, void*, IGameEvent*, bool, bool);
typedef void(__stdcall* tFX_FireBullets)(CBaseCombatWeapon*, int, unsigned short, const Vector&, const QAngle&, int, int, float, float, float, float, int, float);
typedef void(__fastcall* tProcessMovement)(IGameMovement*, void*, CBasePlayer*, CMoveData*);
typedef int(__fastcall* tLogDirect)(void*, void*, int, int, Color, const char*);
typedef bool(__fastcall* tSetSignonState)(void*, void*, int, int, const void*);
typedef CNewParticleEffect*(__fastcall* tCreateNewParticleEffect)(CBaseEntity*, void*, Vector const&, const char*, int);
typedef bool(__fastcall* tSVCMsg_VoiceData)(CClientState*, void*, const CSVCMsg_VoiceData&);
typedef bool(__fastcall* tSendNetMsg)(INetChannel*, void*, INetMessage&, bool, bool);
typedef void(__stdcall* tDrawStaticProps)(void*, void*, const void*, int, bool, bool);
typedef bool(__fastcall* tWriteUserCmdDeltaToBuffer)(CInput*, void*, int, void*, int, int, bool);
typedef bool(__fastcall* tShouldDrawViewModel)(void*, void*);
typedef void(__fastcall* tPerformScreenOverlay)(void*, void*, int, int, int, int);
typedef int(__fastcall* tListLeavesInBox)(void*, void*, const Vector&, const Vector&, unsigned int*, int);
typedef bool(__fastcall* tInPrediction)(IPrediction*, void*);
typedef void(__fastcall* tTraceBlood)(CGameTrace*, int);
typedef void(__fastcall* tCL_DispatchSound)(const SoundInfo_t&, void*);
typedef bool(__fastcall* tInterpolateViewModel)(CBaseEntity*, void*, float);
typedef void(__fastcall* tThrowGrenade)(CBaseGrenade*, void*);
typedef void(__fastcall* tCalcViewModel)(CBaseViewModel*, void*, CBasePlayer*, const Vector&, const QAngle&);
typedef bool(__fastcall* tSVCMsg_TempEntities)(CClientState*, void*, const void*);
typedef void(__fastcall* tResetLatched)(CBasePlayer*, void*);
typedef void(__fastcall* tGetExposureRange)(float*, float*);
typedef void(__fastcall* tEstimateAbsVelocity)(CBaseEntity*, void*, Vector&);
typedef bool(__fastcall* tInterpolatePlayer)(CBasePlayer*, void*, float);
typedef bool(__fastcall* tCIsPaused)(CClientState*, void*);

inline WNDPROC oWndProc;
inline tEndScene oEndScene;
inline tPresent oPresent;
inline tUpdateClientSideAnimation oUpdateClientSideAnimation;
inline tShouldSkipAnimationFrame oShouldSkipAnimationFrame;
inline tDoExtraBoneProcessing oDoExtraBoneProcessing;
inline tBuildTransformations oBuildTransformations;
inline tSetupBones oSetupBones;
inline tCL_Move oCL_Move;
inline tPhysicsSimulate oPhysicsSimulate;
inline tClampBonesInBBox oClampBonesInBBox;
inline tCalculateView oCalculateView;
inline tGetEyeAngles oGetEyeAngles;
inline tRenderSmokeOverlay oRenderSmokeOverlay;
inline tShouldInterpolate oShouldInterpolate;
inline tPacketEnd oPacketEnd;
inline tPacketStart oPacketStart;
inline tProcessMovement oProcessMovement;
inline tLogDirect oLogDirect;
inline tSetSignonState oSetSignonState;
inline tCreateNewParticleEffect oCreateNewParticleEffect;
inline tSVCMsg_VoiceData oSVCMsg_VoiceData;
inline tSendNetMsg oSendNetMsg;
inline tDrawStaticProps oDrawStaticProps;
inline tWriteUserCmdDeltaToBuffer oWriteUserCmdDeltaToBuffer;
inline tShouldDrawViewModel oShouldDrawViewModel;
inline tPerformScreenOverlay oPerformScreenOverlay;
inline tListLeavesInBox oListLeavesInBox;
inline tInPrediction oInPrediction;
inline tTraceBlood oTraceBlood;
inline tCL_DispatchSound oCL_DispatchSound;
inline tInterpolateViewModel oInterpolateViewModel;
inline tThrowGrenade oThrowGrenade;
inline tCalcViewModel oCalcViewModel;
inline tSVCMsg_TempEntities oSVCMsg_TempEntities;
inline tResetLatched oResetLatched;
inline tGetExposureRange oGetExposureRange;
inline tEstimateAbsVelocity oEstimateAbsVelocity;
inline tInterpolatePlayer oInterpolatePlayer;
inline tCIsPaused oCIsPaused;

namespace Hooks {
	inline VMT* DirectXDeviceVMT;
	inline VMT* SurfaceVMT;
	inline VMT* ClientModeVMT;
	inline VMT* PanelVMT;
	inline VMT* EngineVMT;
	inline VMT* ModelRenderVMT;
	inline VMT* ConVarVMT;
	inline VMT* ClientVMT;
	inline VMT* PredictionVMT;
	inline VMT* ModelCacheVMT;
	inline VMT* KeyValuesVMT;

	void Initialize();
	void End();
}