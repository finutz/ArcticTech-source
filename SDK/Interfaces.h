#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "Render.h"
#include "Memory.h"

#include "Interfaces/IClientEntityList.h"
#include "Interfaces/IVEngineClient.h"
#include "Interfaces/IBaseClientDLL.h"
#include "Interfaces/IClientMode.h"
#include "Interfaces/ICvar.h"
#include "Interfaces/IPrediction.h"
#include "Interfaces/CGlobalVarsBase.h"
#include "Interfaces/ISurface.h"
#include "Interfaces/IInputSystem.h"
#include "Interfaces/IPanel.h"
#include "Interfaces/CCSPlayerResource.h"
#include "Interfaces/ILocalize.h"
#include "Interfaces/CGlowObjectManager.h"
#include "Interfaces/IMaterialSystem.h"
#include "Interfaces/CInput.h"
#include "Interfaces/IEngineTrace.h"
#include "Interfaces/IPhysicsSurface.h"
#include "Interfaces/IGameEvents.h"
#include "Interfaces/ViewRenderBeams.h"
#include "Interfaces/IVDebugOverlay.h"
#include "Interfaces/IKeyValuesSystem.h"
#include "Interfaces/IEngineSound.h"
#include "Interfaces/IEffects.h"
#include "Interfaces/NetworkStringTable.h"
#include "Interfaces/ISteamHTTP.h"
#include "Interfaces/CGameRules.h"
#include "Interfaces/CStaticPropManager.h"

extern IClientEntityList* EntityList;
extern IVEngineClient*	  EngineClient;
extern IBaseClientDLL*    Client;
extern IClientMode*		  ClientMode;
extern IDirect3DDevice9*  DirectXDevice;
extern ICvar*			  CVar;
extern IPrediction*       Prediction;
extern CGlobalVarsBase*   GlobalVars;
extern CClientState*	  ClientState;
extern ISurface*		  Surface;
extern IInputSystem*	  InputSystem;
extern IPanel*			  VPanel;
extern CCSPlayerResource* PlayerResource;
extern IWeaponSystem*	  WeaponSystem;
extern ILocalize*		  Localize;
extern CGlowObjectManager*GlowObjectManager;
extern IMaterialSystem*   MaterialSystem;
extern IVModelInfoClient* ModelInfoClient;
extern IVModelRender*	  ModelRender;
extern IStudioRender*	  StudioRender;
extern IVRenderView*      RenderView;
extern CInput*			  Input;
extern IEngineTrace*      EngineTrace;
extern IPhysicsSurfaceProps* PhysicSurfaceProps;
extern IGameEventManager2*GameEventManager;
extern IGameMovement*	  GameMovement;
extern IMoveHelper*		  MoveHelper;
extern IViewRenderBeams*  ViewRenderBeams;
extern IVDebugOverlay*	  DebugOverlay;
extern IMDLCache*		  MDLCache;
extern IKeyValuesSystem*  KeyValuesSystem;
extern IEngineSound*	  EngineSound;
extern IEffects*          Effects;
extern INetworkStringTableContainer* NetworkStringTableContainer;
extern ISteamHTTP*		  SteamHTTP;
extern CStaticPropMgr*	  StaticPropMgr;

namespace Interfaces {
	void Initialize();
}