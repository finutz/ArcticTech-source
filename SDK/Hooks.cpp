#include "Hooks.h"
#include "../Utils/Utils.h"
#include "../Utils/Hash.h"
#include "Config.h"
#include "Globals.h"
#include <intrin.h>
#include "../Utils/Console.h"
#include "../UI/UI.h"
#include "Misc/MoveMsg.h"

#include "../Features/Misc/AutoStrafe.h"
#include "../Features/Visuals/ESP.h"
#include "../Features/Visuals/Glow.h"
#include "../Features/Lua/Bridge/Bridge.h"
#include "../Features/Visuals/Chams.h"
#include "../Features/Visuals/World.h"
#include "../Features/Visuals/GrenadePrediction.h"
#include "../Features/Misc/Prediction.h"
#include "../Features/AntiAim/AntiAim.h"
#include "../Features/RageBot/LagCompensation.h"
#include "../Features/RageBot/Exploits.h"
#include "../Features/Misc/AutoPeek.h"
#include "../Features/RageBot/Ragebot.h"
#include "../Features/RageBot/AutoWall.h"
#include "../Features/RageBot/Resolver.h"
#include "../Features/RageBot/AnimationSystem.h"
#include "../Features/Misc/Misc.h"
#include "../Features/Misc/EventListner.h"
#include "../Features/Visuals/SkinChanger.h"
#include "../Features/ShotManager/ShotManager.h"
#include "../Features/Visuals/PreserveKillfeed.h"

GrenadePrediction NadePrediction;

template <typename T>
inline T HookFunction(void* pTarget, void* pDetour) {
	return (T)DetourFunction((PBYTE)pTarget, (PBYTE)pDetour);
}

inline void RemoveHook(void* original, void* detour) {
	DetourRemove((PBYTE)original, (PBYTE)detour);
}

LRESULT CALLBACK hkWndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (!Render->IsInitialized() || !Menu->IsInitialized())
		return CallWindowProc(oWndProc, Hwnd, Message, wParam, lParam);

	if (Message == WM_KEYDOWN && !EngineClient->Con_IsVisible()) {
		AntiAim->OnKeyPressed(wParam);
	}

	if (Menu->IsOpened() && Menu->WndProc(Hwnd, Message, wParam, lParam))
		return 0;

	return CallWindowProc(oWndProc, Hwnd, Message, wParam, lParam);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* params) {
	static tReset oReset = (tReset)Hooks::DirectXDeviceVMT->GetOriginal(16);

	auto result = oReset(thisptr, params);

	Render->Reset();

	return result;
}

HRESULT __stdcall hkPresent(IDirect3DDevice9* thisptr, const RECT* src, const RECT* dest, HWND window, const RGNDATA* dirtyRegion) {
	if (thisptr != DirectXDevice)
		return oPresent(thisptr, src, dest, window, dirtyRegion);

	if (!Render->IsInitialized()) {
		Render->Init(thisptr);
		Menu->Setup();
		return oPresent(thisptr, src, dest, window, dirtyRegion);
	}

	Cheat.InGame = EngineClient->IsConnected() && EngineClient->IsInGame();

	if (Cheat.InGame && !PlayerResource)
		PlayerResource = **(CCSPlayerResource***)(Utils::PatternScan("client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7", 0x2));
	else if (!Cheat.InGame)
		PlayerResource = nullptr;

	if (thisptr->BeginScene() == D3D_OK) {
		Render->RenderDrawData();
		Menu->Draw();
		thisptr->EndScene();
	}

	return oPresent(thisptr, src, dest, window, dirtyRegion);
}

void __fastcall hkHudUpdate(IBaseClientDLL* thisptr, void* edx, bool bActive) {
	static auto oHudUpdate = (tHudUpdate)Hooks::ClientVMT->GetOriginal(11);

	Cheat.LocalPlayer = (CBasePlayer*)EntityList->GetClientEntity(EngineClient->GetLocalPlayer());

	if (Cheat.LocalPlayer)
		ctx.active_weapon = Cheat.LocalPlayer->GetActiveWeapon();
	else
		ctx.active_weapon = nullptr;

	if (ctx.active_weapon)
		ctx.weapon_info = ctx.active_weapon->GetWeaponInfo();

	if (!Render->IsInitialized() || !Menu->IsInitialized())
		return;

	Render->BeginFrame();

	Render->UpdateViewMatrix(EngineClient->WorldToScreenMatrix());

	ESP::Draw();
	ESP::DrawGrenades();
	ESP::RenderMarkers();

	NadePrediction.Start();
	NadePrediction.Draw();

	AutoPeek->Draw();
	World->Crosshair();
	DebugOverlay->RenderOverlays();

	for (auto& callback : Lua->hooks.getHooks(LUA_RENDER))
		callback.func();

	Render->EndFrame();

	oHudUpdate(thisptr, edx, bActive);
}

void __fastcall hkLockCursor(ISurface* thisptr, void* edx) {
	static tLockCursor oLockCursor = (tLockCursor)Hooks::SurfaceVMT->GetOriginal(67);

	if (Menu->IsOpened())
		return Surface->UnlockCursor();

	oLockCursor(thisptr, edx);
}

MDLHandle_t __fastcall hkFindMdl(void* ecx, void* edx, char* FilePath)
{
	static auto oFindMdlHook = (tFindMdlHook)Hooks::ModelCacheVMT->GetOriginal(10);

	if (strstr(FilePath, "facemask_battlemask.mdl"))
		sprintf(FilePath, "models/player/holiday/facemasks/facemask_dallas.mdl");

	return oFindMdlHook(ecx, edx, FilePath);
}

void __stdcall CreateMove(int sequence_number, float sample_frametime, bool active, bool& bSendPacket) {
	static auto oCHLCCreateMove = (tCHLCCreateMove)Hooks::ClientVMT->GetOriginal(22);

	oCHLCCreateMove(Client, sequence_number, sample_frametime, active);

	CUserCmd* cmd = Input->GetUserCmd(sequence_number);
	CVerifiedUserCmd* verified = Input->GetVerifiedCmd(sequence_number);

	Cheat.LocalPlayer = (CBasePlayer*)EntityList->GetClientEntity(EngineClient->GetLocalPlayer());

	Miscelleaneus::Clantag();

	Exploits->DefenseiveThisTick() = false;
	Exploits->force_charge = false;

	ctx.active_weapon = nullptr;

	if (!cmd || !cmd->command_number || !Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || !active)
		return;

	ctx.active_weapon = Cheat.LocalPlayer->GetActiveWeapon();

	if (ctx.active_weapon)
		ctx.weapon_info = ctx.active_weapon->GetWeaponInfo();

	if (!cmd || !cmd->command_number)
		return;

	ctx.corrected_tickbase = Cheat.LocalPlayer->m_nTickBase() - ctx.tickbase_shift + ctx.lc_exploit;

	ctx.cmd = cmd;
	ctx.send_packet = true;

	CUserCmd_lua lua_cmd;
	lua_cmd.command_number = cmd->command_number;
	lua_cmd.tickcount = cmd->tick_count;
	lua_cmd.move = Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	lua_cmd.viewangles = cmd->viewangles;
	lua_cmd.random_seed = cmd->random_seed;
	lua_cmd.buttons = cmd->buttons;

	for (auto& callback : Lua->hooks.getHooks(LUA_CREATEMOVE)) {
		callback.func(&lua_cmd);
	}

	cmd->buttons = lua_cmd.buttons;
	cmd->sidemove = lua_cmd.move.y;
	cmd->forwardmove = lua_cmd.move.x;
	cmd->tick_count = lua_cmd.tickcount;
	cmd->viewangles = lua_cmd.viewangles;

	if ((config.misc.movement.infinity_duck->get() && (cmd->buttons & IN_DUCK || Cheat.LocalPlayer->m_flDuckAmount() > 0.f)) || config.antiaim.misc.fake_duck->get())
		ctx.cmd->buttons |= IN_BULLRUSH;

	if (config.misc.movement.auto_jump->get()) {
		if (!(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND) && Cheat.LocalPlayer->m_MoveType() != MOVETYPE_NOCLIP && Cheat.LocalPlayer->m_MoveType() != MOVETYPE_LADDER)
			cmd->buttons &= ~IN_JUMP;
	}

	if (config.misc.movement.quick_stop->get() && ((std::abs(cmd->forwardmove) + std::abs(cmd->sidemove)) <= 1.f && !(cmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_JUMP))) && Cheat.LocalPlayer->m_vecVelocity().LengthSqr() > 256 && Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND) {
		Vector vec_speed = Cheat.LocalPlayer->m_vecVelocity();
		QAngle direction = Math::VectorAngles(vec_speed);

		QAngle view; EngineClient->GetViewAngles(&view);
		direction.yaw = view.yaw - direction.yaw;
		direction.Normalize();

		Vector forward;
		Math::AngleVectors(direction, forward);

		Vector nigated_direction = forward * -std::clamp(vec_speed.Q_Length2D(), 0.f, 450.f) * 0.9f;

		cmd->sidemove = nigated_direction.y;
		cmd->forwardmove = nigated_direction.x;
	}

	AutoPeek->CreateMove();
	AntiAim->FakeDuck();
	AntiAim->JitterMove();
	Miscelleaneus::AutoStrafe();
	Miscelleaneus::AutomaticGrenadeRelease();

	EnginePrediction->Start(cmd);
	QAngle eyeYaw = cmd->viewangles;

	if (Exploits->IsShifting()) {
		Ragebot->Run();

		AntiAim->Angles();
		AntiAim->SlowWalk();

		ctx.send_packet = bSendPacket = ctx.tickbase_shift == 1;

		if (ctx.active_weapon && ctx.active_weapon->ShootingWeapon())
			cmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);

		cmd->viewangles.Normalize();
		AnimationSystem->OnCreateMove();

		EnginePrediction->End();

		Utils::FixMovement(cmd, eyeYaw);

		AntiAim->LegMovement();

		ctx.lc_exploit_prev = ctx.lc_exploit;
		if (bSendPacket)
			ctx.lc_exploit = 0;

		if (ctx.lc_exploit != ctx.lc_exploit_prev) {
			ctx.lc_exploit_change = cmd->command_number;
			ctx.lc_exploit_diff = ctx.lc_exploit - ctx.lc_exploit_prev;
		}

		ctx.shifted_commands.emplace_back(cmd->command_number);
		if (bSendPacket)
			ctx.sented_commands.emplace_back(cmd->command_number);

		ctx.teleported_last_tick = true;

		verified->cmd = *cmd;
		verified->crc = cmd->GetChecksum();
		return;
	}

	// pre_prediction

	ctx.is_peeking = AntiAim->IsPeeking();

	if (ctx.is_peeking && config.ragebot.aimbot.doubletap_options->get(1))
		Exploits->DefenseiveThisTick() = true;

	AntiAim->SlowWalk();

	ShotManager->DetectUnregisteredShots();

	Exploits->AllowDefensive() = lua_cmd.allow_defensive;

	if (lua_cmd.override_defensive) {
		Exploits->DefenseiveThisTick() = lua_cmd.override_defensive.as<bool>();
	}

	Exploits->DefensiveDoubletap();

	ctx.last_local_velocity = ctx.local_velocity;
	ctx.local_velocity = Cheat.LocalPlayer->m_vecVelocity();

	Miscelleaneus::CompensateThrowable();

	// prediction

	if (config.misc.movement.edge_jump->get() && !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND) && EnginePrediction->m_fFlags & FL_ONGROUND)
		cmd->buttons |= IN_JUMP;

	if (ctx.active_weapon->ShootingWeapon() && !ctx.active_weapon->CanShoot() && ctx.active_weapon->m_iItemDefinitionIndex() != Revolver)
		cmd->buttons &= ~IN_ATTACK;

	if (ctx.active_weapon->ShootingWeapon() && ctx.active_weapon->CanShoot() && cmd->buttons & IN_ATTACK) {
		if (Exploits->GetExploitType() == CExploits::E_DoubleTap)
			Exploits->ForceTeleport();
		else if (Exploits->GetExploitType() == CExploits::E_HideShots)
			Exploits->HideShot();

		ShotManager->ProcessManualShot();
	}

	AntiAim->lua_override.reset();

	for (auto& cb : Lua->hooks.getHooks(LUA_ANTIAIM))
		cb.func(&AntiAim->lua_override);

	AntiAim->FakeLag();
	AntiAim->Angles();

	Ragebot->Run();

	cmd->viewangles.Normalize(config.misc.miscellaneous.anti_untrusted->get());

	if (Exploits->TeleportThisTick())
		ctx.send_packet = false;

	if (ctx.send_packet) {
		if (!config.antiaim.angles.body_yaw_options->get(1) || Utils::RandomInt(0, 10) >= 6)
			AntiAim->jitter = !AntiAim->jitter;

		ctx.breaking_lag_compensation = (ctx.local_sent_origin - Cheat.LocalPlayer->m_vecOrigin()).LengthSqr() > 4096;
		ctx.local_sent_origin = Cheat.LocalPlayer->m_vecOrigin();
	}

	Utils::FixMovement(cmd, eyeYaw);

	AntiAim->LegMovement();

	Miscelleaneus::FastThrow();

	if (ctx.active_weapon->ShootingWeapon() && ctx.active_weapon->CanShoot() && cmd->buttons & IN_ATTACK) {
		ctx.last_shot_time = GlobalVars->realtime;
		ctx.shot_angles = cmd->viewangles;

		if (!ctx.send_packet) // fix incorrect thirdperson angle when shooting in fakelag
			ctx.force_shot_angle = true;
	}

	AnimationSystem->OnCreateMove();

	EnginePrediction->End();

	// createmove

	ctx.lc_exploit_prev = ctx.lc_exploit;
	if (ctx.send_packet) // exploit state actually changes only when sending packet, so do not update if we are choking
		ctx.lc_exploit = Exploits->LC_TickbaseShift();

	if (ctx.lc_exploit != ctx.lc_exploit_prev) {
		ctx.lc_exploit_change = cmd->command_number;
		ctx.lc_exploit_diff = ctx.lc_exploit - ctx.lc_exploit_prev;
	}

	bSendPacket = ctx.send_packet;
	if (bSendPacket) {
		ctx.sented_commands.emplace_back(cmd->command_number);
		ctx.sent_angles = cmd->viewangles;
	} else {
		auto net_channel = ClientState->m_NetChannel;

		if (net_channel->m_nChokedPackets > 0) {
			auto backup_choke = net_channel->m_nChokedPackets;
			net_channel->m_nChokedPackets = 0;
			net_channel->SendDatagram();
			--net_channel->m_nOutSequenceNr;
			net_channel->m_nChokedPackets = backup_choke;
		}
	}

	if (ctx.should_buy) {
		std::string buy_command = "";
		if (config.misc.miscellaneous.auto_buy->get(0))
			buy_command += "buy awp; ";
		if (config.misc.miscellaneous.auto_buy->get(1))
			buy_command += "buy ssg08; ";
		if (config.misc.miscellaneous.auto_buy->get(2))
			buy_command += "buy scar20; buy g3sg1; ";
		if (config.misc.miscellaneous.auto_buy->get(3))
			buy_command += "buy deagle; buy revolver; ";
		if (config.misc.miscellaneous.auto_buy->get(4))
			buy_command += "buy fn57; buy tec9; ";
		if (config.misc.miscellaneous.auto_buy->get(5))
			buy_command += "buy taser; ";
		if (config.misc.miscellaneous.auto_buy->get(6))
			buy_command += "buy vesthelm; ";
		if (config.misc.miscellaneous.auto_buy->get(7))
			buy_command += "buy smokegrenade; ";
		if (config.misc.miscellaneous.auto_buy->get(8))
			buy_command += "buy molotov; buy incgrenade; ";
		if (config.misc.miscellaneous.auto_buy->get(9))
			buy_command += "buy hegrenade; ";
		if (config.misc.miscellaneous.auto_buy->get(10))
			buy_command += "buy flashbang; ";
		if (config.misc.miscellaneous.auto_buy->get(11))
			buy_command += "buy defuser; ";

		if (!buy_command.empty() && Cheat.LocalPlayer && Cheat.LocalPlayer->m_iAccount() > 1000)
			EngineClient->ExecuteClientCmd(buy_command.c_str());

		ctx.should_buy = false;
	}

	//Console->Log(std::format("{}\t{}\t{}\t{}", bSendPacket, ctx.lc_exploit_change, ctx.lc_exploit, Exploits->charged_command));

	ctx.teleported_last_tick = false;

	verified->cmd = *cmd;
	verified->crc = cmd->GetChecksum();
}

__declspec(naked) void __fastcall hkCHLCCreateMove(IBaseClientDLL* thisptr, void*, int sequence_number, float input_sample_frametime, bool active) {
	__asm {
		push ebp
		mov  ebp, esp
		push ebx
		push esp
		push dword ptr[active]
		push dword ptr[input_sample_frametime]
		push dword ptr[sequence_number]
		call CreateMove
		pop  ebx
		pop  ebp
		retn 0Ch
	}
}

void* __fastcall hkAllocKeyValuesMemory(IKeyValuesSystem* thisptr, void* edx, int iSize)
{
	static auto oAllocKeyValuesMemory = (void*(__fastcall*)(IKeyValuesSystem*, void*, int))Hooks::KeyValuesVMT->GetOriginal(2);

	// return addresses of check function
	// @credits: danielkrupinski
	static const void* uAllocKeyValuesEngine = Utils::PatternScan("engine.dll", "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87 ? ? ? ? 85 F6 0F 84 ? ? ? ?", 0x4A);
	static const void* uAllocKeyValuesClient = Utils::PatternScan("client.dll", "55 8B EC 56 57 8B F9 8B F2 83 FF 11 0F 87 ? ? ? ? 85 F6 0F 84 ? ? ? ?", 0x3E);

	// doesn't call it yet, but have checking function
	//static const std::uintptr_t uAllocKeyValuesMaterialSystem = MEM::FindPattern(MATERIALSYSTEM_DLL, XorStr("FF 52 04 85 C0 74 0C 56")) + 0x3;
	//static const std::uintptr_t uAllocKeyValuesStudioRender = MEM::FindPattern(STUDIORENDER_DLL, XorStr("FF 52 04 85 C0 74 0C 56")) + 0x3;

	if (const void* uReturnAddress = _ReturnAddress(); uReturnAddress == uAllocKeyValuesEngine || uReturnAddress == uAllocKeyValuesClient)
		return nullptr;

	return oAllocKeyValuesMemory(thisptr, edx, iSize);
}

char* __fastcall hk_get_halloween_mask_model_addon( void* ecx, void* edx )
{
	return ( char* )"models/player/holiday/facemasks/facemask_dallas.mdl";
}

bool __fastcall hkSetSignonState(void* thisptr, void* edx, int state, int count, const void* msg) {
	static ConVar* cl_threaded_bone_setup = CVar->FindVar("cl_threaded_bone_setup");

	bool result = oSetSignonState(thisptr, edx, state, count, msg);

	if (state == 6) { // SIGNONSTATE_FULL
		Cheat.InGame = true;

		ctx.update_nightmode = true;
		ctx.update_remove_blood = true;

		World->SkyBox();
		World->Fog();
		World->Smoke();

		cl_threaded_bone_setup->SetInt(1);

		GrenadePrediction::PrecacheParticles();
		Ragebot->CalcSpreadValues();
		ShotManager->Reset();
		Resolver->Reset();
		SkinChanger->InitCustomModels();

		for (auto& callback : Lua->hooks.getHooks(LUA_LEVELINIT))
			callback.func();
	}

	return result;
}

void __fastcall hkLevelShutdown(IBaseClientDLL* thisptr, void* edx) {
	static auto oLevelShutdown = (void(__thiscall*)(IBaseClientDLL*))Hooks::ClientVMT->GetOriginal(7);

	Exploits->target_tickbase_shift = ctx.tickbase_shift = 0;
	ctx.lc_exploit = ctx.lc_exploit_prev = ctx.lc_exploit_diff = 0;
	ctx.reset();
	LagCompensation->Reset();
	AnimationSystem->ResetInterpolation();
	ShotManager->Reset();

	oLevelShutdown(thisptr);
}

void __fastcall hkOverrideView(IClientMode* thisptr, void* edx, CViewSetup* setup) {
	static tOverrideView oOverrideView = (tOverrideView)Hooks::ClientModeVMT->GetOriginal(18);

	if (!Cheat.InGame || !Cheat.LocalPlayer)
		return oOverrideView(thisptr, edx, setup);

	if (config.visuals.effects.fov->get() != 90) {
		float fov = config.visuals.effects.fov->get();;

		if (Cheat.LocalPlayer->IsAlive() && Cheat.LocalPlayer->m_bIsScoped()) {
			CBaseCombatWeapon* weap = Cheat.LocalPlayer->GetActiveWeapon();

			if (weap) {
				switch (weap->m_zoomLevel())
				{
				case 1:
					fov -= (fov - setup->fov) * config.visuals.effects.fov_zoom->get() * 0.01f;
					break;
				case 2:
					fov -= (fov - setup->fov) * config.visuals.effects.fov_second_zoom->get() * 0.01f;
					break;
				default:
					break;
				}
			}
		}

		setup->fov = fov;
	}

	World->ProcessCamera(setup);

	if (Cheat.LocalPlayer && Cheat.LocalPlayer->IsAlive() && config.antiaim.misc.fake_duck->get())
		setup->origin = Cheat.LocalPlayer->GetAbsOrigin() + Vector(0, 0, 64);

	setup->angles.roll = 0;

	oOverrideView(thisptr, edx, setup);
}

void __fastcall hkPaintTraverse(IPanel* thisptr, void* edx, unsigned int panel, bool bForceRepaint, bool bForce) {
	static tPaintTraverse oPaintTraverse = (tPaintTraverse)Hooks::PanelVMT->GetOriginal(41);
	static unsigned int hud_zoom_panel = 0;

	if (!hud_zoom_panel) {
		std::string panelName = VPanel->GetName(panel);

		if (panelName == "HudZoom")
			hud_zoom_panel = panel;
	}

	if (hud_zoom_panel == panel && config.visuals.effects.remove_scope->get() > 0)
		return;

	oPaintTraverse(thisptr, edx, panel, bForceRepaint, bForce);
}

void __fastcall hkDoPostScreenEffects(IClientMode* thisptr, void* edx, CViewSetup* setup) {
	static tDoPostScreenEffects oDoPostScreenEffects = (tDoPostScreenEffects)Hooks::ClientModeVMT->GetOriginal(44);

	Chams->RenderShotChams();
	Glow::Run();

	oDoPostScreenEffects(thisptr, edx, setup);
}

bool __fastcall hkIsPaused(IVEngineClient* thisptr, void* edx) {
	static void* addr = Utils::PatternScan("client.dll", "FF D0 A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? D9 1D ?? ?? ?? ?? FF 50 34 85 C0 74 22 8B 0D ?? ?? ?? ??", 0x29);
	static tIsPaused oIsPaused = (tIsPaused)Hooks::EngineVMT->GetOriginal(90);

	if (_ReturnAddress() == addr)
		return true;

	return oIsPaused(thisptr, edx);
}

void __fastcall hkDrawModelExecute(IVModelRender* thisptr, void* edx, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld) {
	static tDrawModelExecute oDrawModelExecute = (tDrawModelExecute)Hooks::ModelRenderVMT->GetOriginal(21);

	if (hook_info.in_draw_static_props)
		return oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

	if (StudioRender->IsForcedMaterialOverride())
		return oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

	if (Chams->OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld))
		return;

	oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
}

void __fastcall hkFrameStageNotify(IBaseClientDLL* thisptr, void* edx, EClientFrameStage stage) {
	static auto oFrameStageNotify = (tFrameStageNotify)Hooks::ClientVMT->GetOriginal(37);
	Cheat.LocalPlayer = (CBasePlayer*)EntityList->GetClientEntity(EngineClient->GetLocalPlayer());

	static auto CL_ReadPackets = reinterpret_cast<void(__fastcall*)(bool)>(Utils::PatternScan("engine.dll", "53 8A D9 8B 0D ? ? ? ? 56 57 8B B9"));

	switch (stage) {
	case FRAME_RENDER_START: {
		AnimationSystem->RunInterpolation();
		Chams->UpdateSettings();
		World->SunDirection();

		cvars.r_aspectratio->SetFloat(config.visuals.effects.aspect_ratio->get());
		cvars.mat_postprocessing_enable->SetInt(!config.visuals.effects.removals->get(0));
		cvars.cl_csm_shadows->SetInt(!config.visuals.effects.removals->get(2));
		cvars.cl_foot_contact_shadows->SetInt(0);
		cvars.r_drawsprites->SetInt(!config.visuals.effects.removals->get(6));
		cvars.zoom_sensitivity_ratio_mouse->SetInt(!config.visuals.effects.removals->get(5));
		SkinChanger->Run(false);

		if (Cheat.LocalPlayer && config.visuals.effects.removals->get(4))
			Cheat.LocalPlayer->m_flFlashDuration() = 0.f;

		break;
	}
	case FRAME_RENDER_END: {
		if (ctx.update_nightmode) {
			World->Modulation();
			ctx.update_nightmode = false;
		}
		SkinChanger->Run(true);

		if (ctx.update_remove_blood) {
			World->RemoveBlood();
			ctx.update_remove_blood = false;
		}

		//GlobalVars->store();
		//hook_info.read_packets = true;
		//CL_ReadPackets(false);
		//hook_info.read_packets = false;
		//GlobalVars->restore();

		break;
	}
	case FRAME_NET_UPDATE_START:
		ShotManager->OnNetUpdate();
		break;
	case FRAME_NET_UPDATE_END:
		LagCompensation->OnNetUpdate();
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		SkinChanger->AgentChanger();
		SkinChanger->MaskChanger();
		SkinChanger->Run(true);
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		break;
	}

	AnimationSystem->FrameStageNotify(stage);

	oFrameStageNotify(thisptr, edx, stage);
}

void __fastcall hkUpdateClientSideAnimation(CBasePlayer* thisptr, void* edx) {
	if (hook_info.update_csa)
		return oUpdateClientSideAnimation(thisptr, edx);
	if (!thisptr->IsPlayer())
		return oUpdateClientSideAnimation(thisptr, edx);

	if (thisptr == Cheat.LocalPlayer) {
		CCSGOPlayerAnimationState* animstate = thisptr->GetAnimstate();

		CBasePlayer* backup_player = nullptr;

		if (animstate) {
			backup_player = animstate->pEntity;
			animstate->pEntity = nullptr; // do not update animstate
		}

		oUpdateClientSideAnimation(thisptr, edx);

		if (animstate)
			animstate->pEntity = backup_player;
	}
}

bool __fastcall hkShouldSkipAnimationFrame(CBasePlayer* thisptr, void* edx) {
	if (!thisptr->IsPlayer())
		return oShouldSkipAnimationFrame(thisptr, edx);

	if (Cheat.LocalPlayer == thisptr || !thisptr->IsTeammate())
		return false;

	return oShouldSkipAnimationFrame(thisptr, edx);
}

bool __fastcall hkShouldInterpolate(CBasePlayer* thisptr, void* edx) {
	if (!Exploits->ShouldCharge() || thisptr != Cheat.LocalPlayer)
		return oShouldInterpolate(thisptr, edx);

	AnimationSystem->DisableInterpolationFlags(thisptr);

	return false;
}

void __fastcall hkDoExtraBoneProcessing(CBaseEntity* player, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* contex) {

}

bool __fastcall hkIsHLTV(IVEngineClient* thisptr, void* edx) {
	static auto oIsHLTV = (tIsHLTV)Hooks::EngineVMT->GetOriginal(93);

	if (hook_info.setup_bones || hook_info.update_csa)
		return true;

	static const auto setup_velocity = Utils::PatternScan("client.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0");
	static const auto accumulate_layers = Utils::PatternScan("client.dll", "84 C0 75 0D F6 87");
	static const auto reevalute_anim_lod = Utils::PatternScan("client.dll", "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7");

	if (_ReturnAddress() == setup_velocity || _ReturnAddress() == accumulate_layers || _ReturnAddress() == reevalute_anim_lod)
		return true;

	return oIsHLTV(thisptr, edx);
}

void __fastcall hkBuildTransformations(CBaseEntity* thisptr, void* edx, void* hdr, void* pos, void* q, const void* camera_transform, int bone_mask, void* bone_computed) {
	if (thisptr)
		thisptr->m_isJiggleBonesEnabled() = false;

	oBuildTransformations(thisptr, edx, hdr, pos, q, camera_transform, bone_mask, bone_computed);
}

bool __fastcall hkSetupBones(CBaseEntity* thisptr, void* edx, matrix3x4_t* pBoneToWorld, int maxBones, int mask, float curTime) {
	if (hook_info.setup_bones || !thisptr)
		return oSetupBones(thisptr, edx, pBoneToWorld, maxBones, mask, curTime);

	CBasePlayer* ent = (CBasePlayer*)((uintptr_t)thisptr - 0x4);

	if (!ent->IsPlayer() || !ent->IsAlive())
		return oSetupBones(thisptr, edx, pBoneToWorld, maxBones, mask, curTime);

	if (ent == Cheat.LocalPlayer) {
		memcpy(ent->GetCachedBoneData().Base(), AnimationSystem->GetLocalBoneMatrix(), ent->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
		AnimationSystem->CorrectLocalMatrix(ent->GetCachedBoneData().Base(), ent->GetCachedBoneData().Count());

		if (mask & BONE_USED_BY_ATTACHMENT)
			Cheat.LocalPlayer->SetupBones_AttachmentHelper();

		if (pBoneToWorld && maxBones != -1) {
			ent->CopyBones(pBoneToWorld);
		}

		return true;
	}

	if (!hook_info.disable_interpolation && mask & BONE_USED_BY_ATTACHMENT) {
		AnimationSystem->InterpolateModel(ent, ent->GetCachedBoneData().Base());

		ent->SetAbsOrigin(AnimationSystem->GetInterpolated(ent));
		ent->SetupBones_AttachmentHelper();
	}

	if (pBoneToWorld && maxBones != -1) {
		ent->CopyBones(pBoneToWorld);
	}

	return true;
}

void __fastcall hkRunCommand(IPrediction* thisptr, void* edx, CBasePlayer* player, CUserCmd* cmd, IMoveHelper* moveHelper) {
	static auto oRunCommand = (tRunCommand)(Hooks::PredictionVMT->GetOriginal(19));

	if (!player || !cmd || player != Cheat.LocalPlayer)
		return oRunCommand(thisptr, edx, player, cmd, moveHelper);

	int& tickbase = player->m_nTickBase();

	if (cmd->tick_count == INT_MAX) {
		tickbase++;
		return;
	}

	if (ctx.lc_exploit_change == cmd->command_number)
		tickbase -= ctx.lc_exploit_diff;

	if (cmd->command_number == Exploits->charged_command + 1)
		tickbase += ctx.shifted_last_tick;

	const int backup_tickbase = tickbase;
	const float backup_velocity_modifier = player->m_flVelocityModifier();

	oRunCommand(thisptr, edx, player, cmd, moveHelper);

	player->m_flVelocityModifier() = backup_velocity_modifier;

	for (auto i = ctx.shifted_commands.begin(); i != ctx.shifted_commands.end();) {
		auto command = *i;

		if (cmd->command_number - command > 32) {
			i = ctx.shifted_commands.erase(i);
			continue;
		}

		if (command == cmd->command_number)
			tickbase = backup_tickbase;

		++i;
	}

	MoveHelper = moveHelper;
}

void __fastcall hkPhysicsSimulate(CBasePlayer* thisptr, void* edx) {
	const int tick_base = thisptr->m_nTickBase();
	C_CommandContext* c_ctx = thisptr->GetCommandContext();

	if (thisptr != Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || thisptr->m_nSimulationTick() == GlobalVars->tickcount || !c_ctx->needsprocessing)
		return oPhysicsSimulate(thisptr, edx);

	oPhysicsSimulate(thisptr, edx);

	EnginePrediction->StoreNetvars(c_ctx->cmd.command_number);
}

void __fastcall hkPacketStart(CClientState* thisptr, void* edx, int incoming_sequence, int outgoing_acknowledged) {
	if (!Cheat.InGame || !Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive())
		return oPacketStart(thisptr, edx, incoming_sequence, outgoing_acknowledged);

	for (auto it = ctx.sented_commands.begin(); it != ctx.sented_commands.end(); it++) {
		if (*it == outgoing_acknowledged) {
			oPacketStart(thisptr, edx, incoming_sequence, outgoing_acknowledged);
			break;
		}
	}

	ctx.sented_commands.erase(
		std::remove_if(
			ctx.sented_commands.begin(),
			ctx.sented_commands.end(),
			[&](auto const& command) { return std::abs(command - outgoing_acknowledged) >= 150 || command < outgoing_acknowledged; }),
		ctx.sented_commands.end());
}

void __fastcall hkPacketEnd(CClientState* thisptr, void* edx) {
	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive())
		return oPacketEnd(thisptr, edx);

	if (ClientState->m_ClockDriftMgr.m_nServerTick == ClientState->m_nDeltaTick)
		EnginePrediction->RestoreNetvars(ClientState->m_nLastOutgoingCommand);

	oPacketEnd(thisptr, edx);
}

void __fastcall hkClampBonesInBBox(CBasePlayer* thisptr, void* edx, matrix3x4_t* bones, int boneMask) {
	if (config.antiaim.angles.legacy_desync->get() || hook_info.disable_clamp_bones)
		return;

	auto backup_curtime = GlobalVars->curtime;
	if (thisptr == Cheat.LocalPlayer) {
		GlobalVars->curtime = EnginePrediction->curtime();
	}

	if (thisptr->m_fFlags() & FL_FROZEN) {
		thisptr->SetCollisionBounds(Vector(-16, -16, 0), Vector(16, 16, 72));
	}

	oClampBonesInBBox(thisptr, edx, bones, boneMask);

	GlobalVars->curtime = backup_curtime;
}

void __cdecl hkCL_Move(float accamulatedExtraSamples, bool bFinalTick) {
	Cheat.LocalPlayer = (CBasePlayer*)EntityList->GetClientEntity(EngineClient->GetLocalPlayer());

	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive())
		return oCL_Move(accamulatedExtraSamples, bFinalTick);

	Exploits->Run();

	if (Exploits->ShouldCharge()) {
		ctx.tickbase_shift++;
		ctx.shifted_last_tick++;

		return;
	}

	oCL_Move(accamulatedExtraSamples, bFinalTick);

	Exploits->HandleTeleport(oCL_Move);
}

QAngle* __fastcall hkGetEyeAngles(CBasePlayer* thisptr, void* edx) {
	if (thisptr != Cheat.LocalPlayer)
		return oGetEyeAngles(thisptr, edx);

	static void* ShitAnimRoll = Utils::PatternScan("client.dll", "8B 55 0C 8B C8 E8 ? ? ? ? 83 C4 08 5E 8B E5");

	if (_ReturnAddress() != ShitAnimRoll)
		return oGetEyeAngles(thisptr, edx);

	return &ctx.setup_bones_angle;
}

bool __fastcall hkSendNetMsg(INetChannel* thisptr, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice) {
	if (msg.GetType() == 14) // Return and don't send messsage if its FileCRCCheck
		return true;

	if (msg.GetGroup() == 9) // Fix lag when transmitting voice and fakelagging
		bVoice = true;

	return oSendNetMsg(thisptr, edx, msg, bForceReliable, bVoice);
}

void __fastcall hkCalculateView(CBasePlayer* thisptr, void* edx, Vector& eyeOrigin, QAngle& eyeAngle, float& z_near, float& z_far, float& fov) {
	if (!thisptr || thisptr != Cheat.LocalPlayer)
		return oCalculateView(thisptr, edx, eyeOrigin, eyeAngle, z_near, z_far, fov);

	static const uintptr_t m_bUseNewAnimstate_offset = *(uintptr_t*)Utils::PatternScan("client.dll", "80 BE ? ? ? ? ? 0F 84 ? ? ? ? 83 BE ? ? ? ? ? 0F 84", 0x2);

	bool& m_bUseNewAnimstate = *(bool*)(thisptr + m_bUseNewAnimstate_offset);
	const bool backup = m_bUseNewAnimstate;

	m_bUseNewAnimstate = false;
	oCalculateView(thisptr, edx, eyeOrigin, eyeAngle, z_near, z_far, fov);
	m_bUseNewAnimstate = backup;
}

void __fastcall hkRenderSmokeOverlay(void* thisptr, void* edx, bool bPreViewModel) {
	if (!config.visuals.effects.removals->get(3))
		oRenderSmokeOverlay(thisptr, edx, bPreViewModel);
}

void __fastcall hkProcessMovement(IGameMovement* thisptr, void* edx, CBasePlayer* player, CMoveData* mv) {
	mv->bGameCodeMovedPlayer = false;
	oProcessMovement(thisptr, edx, player, mv);
}

int __fastcall hkLogDirect(void* loggingSystem, void* edx, int channel, int serverity, Color color, const char* text) {
	if (hook_info.console_log)
		return oLogDirect(loggingSystem, edx, channel, serverity, color, text);

	if (!config.misc.miscellaneous.filter_console->get())
		return oLogDirect(loggingSystem, edx, channel, serverity, color, text);

	return 0;
}

CNewParticleEffect* hkCreateNewParticleEffect(void* pDef, CBaseEntity* pOwner, Vector const& vecAggregatePosition, const char* pDebugName, int nSplitScreenUser) {
	CNewParticleEffect* result;

	__asm {
		mov edx, pDef
		push nSplitScreenUser
		push pDebugName
		push vecAggregatePosition
		push pOwner
		call oCreateNewParticleEffect
		mov result, eax
	}

	if (IEFFECTS::bCaptureEffect)
		IEFFECTS::pCapturedEffect = result;

	return result;
}

__declspec(naked) void hkCreateNewParticleEffect_proxy() {
	__asm
	{
		push edx
		call hkCreateNewParticleEffect
		pop edx
		retn
	}
}

bool __fastcall hkSVCMsg_VoiceData(CClientState* clientstate, void* edx, const CSVCMsg_VoiceData& msg) {
	if (NetMessages->OnVoiceDataRecieved(msg))
		return true;

	return oSVCMsg_VoiceData(clientstate, edx, msg);
}

void __stdcall hkDrawStaticProps(void* thisptr, IClientRenderable** pProps, const void* pInstances, int count, bool bShadowDepth, bool drawVCollideWireframe) {
	hook_info.in_draw_static_props = true;
	oDrawStaticProps(thisptr, pProps, pInstances, count, bShadowDepth, drawVCollideWireframe);
	hook_info.in_draw_static_props = false;
}

bool __fastcall hkWriteUserCmdDeltaToBuffer(CInput* thisptr, void* edx, int slot, void* buf, int from, int to, bool isnewcommand) {
	if (!Cheat.InGame || !Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || !ctx.lc_exploit || !ctx.tickbase_shift)
		return oWriteUserCmdDeltaToBuffer(thisptr, edx, slot, buf, from, to, isnewcommand);

	if (from != -1)
		return true;

	uintptr_t* stack_pointer;
	__asm mov stack_pointer, ebp;

	CCLCMsg_Move_t* moveMsg = reinterpret_cast<CCLCMsg_Move_t*>(*stack_pointer - 0x58);

	auto new_commands = moveMsg->new_commands + moveMsg->backup_commands;
	auto next_cmd_nr = ClientState->m_nLastOutgoingCommand + ClientState->m_nChokedCommands + 1;

	moveMsg->new_commands = std::clamp(moveMsg->new_commands + ctx.lc_exploit, 1, 15);
	//moveMsg->backup_commands = 0;

	for (to = next_cmd_nr - new_commands + 1; to <= next_cmd_nr; to++) {
		if (!oWriteUserCmdDeltaToBuffer(thisptr, edx, slot, buf, from, to, true))
			return false;

		from = to;
	}

	CUserCmd* user_cmd = Input->GetUserCmd(from);

	if (!user_cmd)
		return true;

	CUserCmd from_cmd;
	CUserCmd to_cmd;

	from_cmd = *user_cmd;
	to_cmd = from_cmd;

	to_cmd.command_number++;
	to_cmd.tick_count = INT_MAX;

	for (int i = 0; i < ctx.lc_exploit; i++) {
		WriteUserCmd(buf, &to_cmd, &from_cmd);

		from_cmd = to_cmd;
		to_cmd.command_number++;
	}

	return true;
}

bool __fastcall hkShouldDrawViewModel(void* thisptr, void* edx) {
	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || config.visuals.effects.viewmodel_scope_alpha->get() == 0)
		return oShouldDrawViewModel(thisptr, edx);

	return true;
}

void __fastcall hkPerformScreenOverlay(void* viewrender, void* edx, int x, int y, int w, int h) {
	if (config.misc.miscellaneous.ad_block->get())
		return;

	oPerformScreenOverlay(viewrender, edx, x, y, w, h);
}

int __fastcall hkListLeavesInBox(void* ecx, void* edx, const Vector& mins, const Vector& maxs, unsigned int* list, int size) {
	static void* insert_into_tree = Utils::PatternScan("client.dll", "56 52 FF 50 18", 0x5);

	if (!config.visuals.chams.disable_model_occlusion->get() || _ReturnAddress() != insert_into_tree)
		return oListLeavesInBox(ecx, edx, mins, maxs, list, size);

	// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
	if (!info || !info->m_pRenderable)
		return oListLeavesInBox(ecx, edx, mins, maxs, list, size);

	// check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
	auto base_entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();
	if (!base_entity || !base_entity->IsPlayer())
		return oListLeavesInBox(ecx, edx, mins, maxs, list, size);

	// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
	// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
	info->m_Flags &= ~0x100;
	info->m_bRenderInFastReflection |= 0xC0;

	// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
	static const Vector map_min = Vector(-16384.0f, -16384.0f, -16384.0f);
	static const Vector map_max = Vector(16384.0f, 16384.0f, 16384.0f);

	return oListLeavesInBox(ecx, edx, map_min, map_max, list, size);
}

bool __fastcall hkInPrediction(IPrediction* ecx, void* edx) {
	static auto setup_bones = Utils::PatternScan("client.dll", "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05");

	if (_ReturnAddress() == setup_bones)
		return false;

	return oInPrediction(ecx, edx);
}

void __fastcall hkCL_DispatchSound(const SoundInfo_t& snd, void* edx) {
	ESP::ProcessSound(snd);
	oCL_DispatchSound(snd, edx);
}

bool __fastcall hkInterpolateViewModel(CBaseViewModel* vm, void* edx, float curTime) {
	if (EntityList->GetClientEntityFromHandle(vm->m_hOwner()) != Cheat.LocalPlayer)
		return oInterpolateViewModel(vm, edx, curTime);

	auto backup_pred_tick = Cheat.LocalPlayer->m_nFinalPredictedTick();
	auto backup_lerp_amt = GlobalVars->interpolation_amount;

	Cheat.LocalPlayer->m_nFinalPredictedTick() = EnginePrediction->tickcount();
	if (Exploits->ShouldCharge())
		GlobalVars->interpolation_amount = 0.f;

	auto result = oInterpolateViewModel(vm, edx, curTime);
	Cheat.LocalPlayer->m_nFinalPredictedTick() = backup_pred_tick;

	GlobalVars->interpolation_amount = backup_lerp_amt;

	return result;
}

bool __fastcall hkInterpolatePlayer(CBasePlayer* pl, void* edx, float curTime) {
	if (pl != Cheat.LocalPlayer || !pl)
		return oInterpolatePlayer(pl, edx, curTime);

	int& pred_tick = pl->m_nFinalPredictedTick();
	auto backup_pred_tick = pred_tick;

	if (Exploits->ShouldCharge())
		pred_tick = Cheat.LocalPlayer->m_nTickBase();

	bool result = oInterpolatePlayer(pl, edx, curTime);

	pred_tick = backup_pred_tick;

	return result;
}

void __fastcall hkThrowGrenade(CBaseGrenade* thisptr, void* edx) {
	if (EntityList->GetClientEntityFromHandle(thisptr->m_hOwnerEntity()) == Cheat.LocalPlayer && ctx.cmd)
		ctx.grenade_throw_tick = ctx.cmd->command_number;

	oThrowGrenade(thisptr, edx);
}

void __fastcall hkCalcViewModel(CBaseViewModel* vm, void* edx, CBasePlayer* player, const Vector& eyePosition, const QAngle& eyeAngles) {
	if (ctx.fake_duck && Cheat.LocalPlayer == player)
		return oCalcViewModel(vm, edx, player, player->GetAbsOrigin() + Vector(0, 0, 64), eyeAngles);

	oCalcViewModel(vm, edx, player, eyePosition, eyeAngles);
}

bool __fastcall hkSVCMsg_TempEntities(CClientState* thisptr, void* edx, const void* msg) {
	auto old_maxclients = thisptr->m_nMaxClients;

	// nope https://github.com/perilouswithadollarsign/cstrike15_src/blob/HEAD/engine/servermsghandler.cpp#L817
	thisptr->m_nMaxClients = 1;
	bool result = oSVCMsg_TempEntities(thisptr, edx, msg);
	thisptr->m_nMaxClients = old_maxclients;

	EngineClient->FireEvents();

	return result;
}

void __fastcall hkResetLatched(CBasePlayer* thisptr, void* edx) {
	if (!Cheat.LocalPlayer || thisptr != Cheat.LocalPlayer || EnginePrediction->HasPredictionErrors())
		return oResetLatched(thisptr, edx);
}

void __fastcall hkGetExposureRange(float* min, float* max) {
	*min = 1.f;
	*max = 1.f;

	oGetExposureRange(min, max);
}

void __fastcall hkEstimateAbsVelocity(CBaseEntity* ent, void* edx, Vector& vel) {
	if (ent && ent->IsPlayer()) {
		auto pl = reinterpret_cast<CBasePlayer*>(ent);
		if (pl->IsTeammate())
			return oEstimateAbsVelocity(ent, edx, vel);

		vel = pl->m_vecVelocity();

		return;
	}

	oEstimateAbsVelocity(ent, edx, vel);
}

bool __fastcall hkCIsPaused(CClientState* state, void* edx) {
	static auto cl_readpackets = Utils::PatternScan("engine.dll", "84 C0 75 ? FF 86");

	if (_ReturnAddress() == cl_readpackets && hook_info.read_packets)
		return true;

	return oCIsPaused(state, edx);
}

void Hooks::Initialize() {
	oWndProc = (WNDPROC)(SetWindowLongPtr(FindWindowA("Valve001", nullptr), GWL_WNDPROC, (LONG_PTR)hkWndProc));

	ESP::RegisterCallback();
	Chams->LoadChams();
	SkinChanger->LoadKnifeModels();
	Ragebot->CreateThreads();

	DirectXDeviceVMT = new VMT(DirectXDevice);
	SurfaceVMT = new VMT(Surface);
	ClientModeVMT = new VMT(ClientMode);
	PanelVMT = new VMT(VPanel);
	EngineVMT = new VMT(EngineClient);
	ModelRenderVMT = new VMT(ModelRender);
	ClientVMT = new VMT(Client);
	PredictionVMT = new VMT(Prediction);
	ModelCacheVMT = new VMT(MDLCache);
	KeyValuesVMT = new VMT(KeyValuesSystem);

	// vmt hooking for directx doesnt work for some reason
	oPresent = HookFunction<tPresent>(Utils::PatternScan("gameoverlayrenderer.dll", "55 8B EC 83 EC 4C 53"), hkPresent);
	//oReset = HookFunction<tReset>(Utils::PatternScan("d3d9.dll", "8B FF 55 8B EC 83 E4 F8 81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 53 8B 5D 08 8B CB"), hkReset);

	while (!Menu->IsInitialized())
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	DirectXDeviceVMT->Hook(16, hkReset);
	SurfaceVMT->Hook(67, hkLockCursor);
	ClientModeVMT->Hook(18, hkOverrideView);
	ClientModeVMT->Hook(44, hkDoPostScreenEffects);
	PanelVMT->Hook(41, hkPaintTraverse);
	EngineVMT->Hook(90, hkIsPaused);
	EngineVMT->Hook(93, hkIsHLTV);
	ModelRenderVMT->Hook(21, hkDrawModelExecute);
	ClientVMT->Hook(37, hkFrameStageNotify);
	ClientVMT->Hook(11, hkHudUpdate);
	ClientVMT->Hook(22, hkCHLCCreateMove);
	//ModelCacheVMT->Hook(10 ,hkFindMdl);
	ClientVMT->Hook(7, hkLevelShutdown);
	PredictionVMT->Hook(19, hkRunCommand);
	KeyValuesVMT->Hook(2, hkAllocKeyValuesMemory);

	oUpdateClientSideAnimation = HookFunction<tUpdateClientSideAnimation>(Utils::PatternScan("client.dll", "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74"), hkUpdateClientSideAnimation);
	oDoExtraBoneProcessing = HookFunction<tDoExtraBoneProcessing>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"), hkDoExtraBoneProcessing);
	oShouldSkipAnimationFrame = HookFunction<tShouldSkipAnimationFrame>(Utils::PatternScan("client.dll", "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"), hkShouldSkipAnimationFrame);
	oBuildTransformations = HookFunction<tBuildTransformations>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F0 81 ? ? ? ? ? 56 57 8B F9 8B"), hkBuildTransformations);
	oSetupBones = HookFunction<tSetupBones>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57"), hkSetupBones);
	oCL_Move = HookFunction<tCL_Move>(Utils::PatternScan("engine.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8A F9 F3 0F 11 45 ? 8B 4D 04"), hkCL_Move);
	oPhysicsSimulate = HookFunction<tPhysicsSimulate>(Utils::PatternScan("client.dll", "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23 0F B7 C1 C1 E0 04 05 ? ? ? ?"), hkPhysicsSimulate);
	oClampBonesInBBox = HookFunction<tClampBonesInBBox>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38"), hkClampBonesInBBox);
	oCalculateView = HookFunction<tCalculateView>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 14 53 56 57 FF 75 18"), hkCalculateView);
	oSendNetMsg = HookFunction<tSendNetMsg>(Utils::PatternScan("engine.dll", "F1 8B 4D 04 E8 ? ? ? ? 8B 86 ? ? ? ? 85 C0 74 24 48 83 F8 02 77 2C 83 BE ? ? ? ? ? 8D 8E ? ? ? ? 74 06 32 C0 84 C0", -0x8), hkSendNetMsg);
	oGetEyeAngles = HookFunction<tGetEyeAngles>(Utils::PatternScan("client.dll", "56 8B F1 85 F6 74 32"), hkGetEyeAngles);
	oRenderSmokeOverlay = HookFunction<tRenderSmokeOverlay>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 30 80 7D 08 00"), hkRenderSmokeOverlay);
	oShouldInterpolate = HookFunction<tShouldInterpolate>(Utils::PatternScan("client.dll", "56 8B F1 E8 ? ? ? ? 3B F0"), hkShouldInterpolate);
	oPacketStart = HookFunction<tPacketStart>(Utils::PatternScan("engine.dll", "55 8B EC 8B 45 08 89 81 ? ? ? ? 8B 45 0C 89 81 ? ? ? ? 5D C2 08 00 CC CC CC CC CC CC CC 56"), hkPacketStart);
	oPacketEnd = HookFunction<tPacketEnd>(Utils::PatternScan("engine.dll", "56 8B F1 E8 ? ? ? ? 8B 8E ? ? ? ? 3B 8E ? ? ? ? 75 34"), hkPacketEnd);
	//oFX_FireBullets = HookFunction<tFX_FireBullets>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 C0 F3 0F 10 45"), hkFX_FireBullets);
	oProcessMovement = HookFunction<tProcessMovement>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 C0 83 EC 38 A1 ? ? ? ?"), hkProcessMovement);
	oLogDirect = HookFunction<tLogDirect>(Utils::PatternScan("tier0.dll", "55 8B EC 83 E4 F8 8B 45 08 83 EC 14 53 56 8B F1 57 85 C0 0F 88 ? ? ? ?"), hkLogDirect);
	oSetSignonState = HookFunction<tSetSignonState>(Utils::PatternScan("engine.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57 FF 75 10"), hkSetSignonState);
	oCreateNewParticleEffect = HookFunction<tCreateNewParticleEffect>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 0C 53 56 8B F2 89 75 F8 57"), hkCreateNewParticleEffect_proxy);
	oSVCMsg_VoiceData = HookFunction<tSVCMsg_VoiceData>(Utils::PatternScan("engine.dll", "55 8B EC 83 E4 F8 A1 ? ? ? ? 81 EC ? ? ? ? 53 56 8B F1 B9 ? ? ? ? 57 FF 50 34 8B 7D 08 85 C0 74 13 8B 47 08 40 50"), hkSVCMsg_VoiceData);
	oDrawStaticProps = HookFunction<tDrawStaticProps>(Utils::PatternScan("engine.dll", "55 8B EC 56 57 8B F9 8B 0D ? ? ? ? 8B B1 ? ? ? ? 85 F6 74 16 6A 04 6A 00 68"), hkDrawStaticProps);
	oWriteUserCmdDeltaToBuffer = HookFunction<tWriteUserCmdDeltaToBuffer>(Utils::PatternScan("client.dll", "55 8B EC B9 ? ? ? ? A1 ? ? ? ? 8B 40 14"), hkWriteUserCmdDeltaToBuffer);
	oShouldDrawViewModel = HookFunction<tShouldDrawViewModel>(Utils::PatternScan("client.dll", "55 8B EC 51 57 E8"), hkShouldDrawViewModel);
	oPerformScreenOverlay = HookFunction<tPerformScreenOverlay>(Utils::PatternScan("client.dll", "55 8B EC 51 A1 ? ? ? ? 53 56 8B D9 B9 ? ? ? ? 57 89 5D FC FF 50 34 85 C0 75 36"), hkPerformScreenOverlay);
	oListLeavesInBox = HookFunction<tListLeavesInBox>(Utils::PatternScan("engine.dll", "55 8B EC 83 EC 18 8B 4D 0C"), hkListLeavesInBox);
	oInPrediction = HookFunction<tInPrediction>(Utils::PatternScan("client.dll", "8A 41 08 C3"), hkInPrediction);
	oCL_DispatchSound = HookFunction<tCL_DispatchSound>(Utils::PatternScan("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 8B F1 8D 4D 98 E8"), hkCL_DispatchSound);
	oInterpolateViewModel = HookFunction<tInterpolateViewModel>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 57 83 BE"), hkInterpolateViewModel);
	oThrowGrenade = HookFunction<tThrowGrenade>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57 8B F9 80 BF ? ? ? ? ? 74 07"), hkThrowGrenade);
	oCalcViewModel = HookFunction<tCalcViewModel>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 58 56 57"), hkCalcViewModel);
	oSVCMsg_TempEntities = HookFunction<tSVCMsg_TempEntities>(Utils::PatternScan("engine.dll", "55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80"), hkSVCMsg_TempEntities);
	oResetLatched = HookFunction<tResetLatched>(Utils::PatternScan("client.dll", "56 8B F1 57 8B BE ? ? ? ? 85 FF 74 ? 8B CF E8 ? ? ? ? 68"), hkResetLatched);
	oGetExposureRange = HookFunction<tGetExposureRange>(Utils::PatternScan("client.dll", "55 8B EC 51 80 3D ? ? ? ? ? 0F 57"), hkGetExposureRange);
	oEstimateAbsVelocity = HookFunction<tEstimateAbsVelocity>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 ? 83 EC ? 56 8B F1 85 F6 74 ? 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 ? 8A 86"), hkEstimateAbsVelocity);
	//oInterpolatePlayer = HookFunction<tInterpolatePlayer>(Utils::PatternScan("client.dll", "55 8B EC 83 EC ? 56 8B F1 83 BE ? ? ? ? ? 0F 85"), hkInterpolatePlayer);
	oCIsPaused = HookFunction<tCIsPaused>(Utils::PatternScan("engine.dll", "80 B9 ? ? ? ? ? 75 ? 80 3D"), hkCIsPaused);

	EventListner->Register();

	Memory->BytePatch(Utils::PatternScan("client.dll", "75 30 38 87"), { 0xEB }); // CameraThink sv_cheats check skip
}

void Hooks::End() {
	SetWindowLongPtr(FindWindowA("Valve001", nullptr), GWL_WNDPROC, (LONG_PTR)oWndProc);

	EventListner->Unregister();
	Ragebot->TerminateThreads();
	Lua->UnloadAll();
	Menu->Release();

	DirectXDeviceVMT->UnHook(16);
	SurfaceVMT->UnHook(67);
	ClientModeVMT->UnHook(18);
	ClientModeVMT->UnHook(44);
	PanelVMT->UnHook(41);
	EngineVMT->UnHook(90);
	EngineVMT->UnHook(93);
	ModelRenderVMT->UnHook(21);
	ClientVMT->UnHook(37);
	ClientVMT->UnHook(11);
	ClientVMT->UnHook(22);
	ClientVMT->UnHook(7);
	PredictionVMT->UnHook(19);
	//ClientVMT->UnHook(40);
	KeyValuesVMT->UnHook(2);
	ModelCacheVMT->UnHook(10);

	RemoveHook(oPresent, hkPresent);
	//RemoveHook(oReset, hkReset);
	RemoveHook(oUpdateClientSideAnimation, hkUpdateClientSideAnimation);
	RemoveHook(oDoExtraBoneProcessing, hkDoExtraBoneProcessing);
	RemoveHook(oShouldSkipAnimationFrame, hkShouldSkipAnimationFrame);
	RemoveHook(oBuildTransformations, hkBuildTransformations);
	RemoveHook(oSetupBones, hkSetupBones);
	RemoveHook(oCL_Move, hkCL_Move);
	RemoveHook(oPhysicsSimulate, hkPhysicsSimulate);
	RemoveHook(oClampBonesInBBox, hkClampBonesInBBox);
	RemoveHook(oCalculateView, hkCalculateView);
	RemoveHook(oSendNetMsg, hkSendNetMsg);
	RemoveHook(oGetEyeAngles, hkGetEyeAngles);
	RemoveHook(oRenderSmokeOverlay, hkRenderSmokeOverlay);
	RemoveHook(oShouldInterpolate, hkShouldInterpolate);
	RemoveHook(oPacketStart, hkPacketStart);
	RemoveHook(oPacketEnd, hkPacketEnd);
	//RemoveHook(oFX_FireBullets, hkFX_FireBullets);
	RemoveHook(oProcessMovement, hkProcessMovement);
	RemoveHook(oLogDirect, hkLogDirect);
	RemoveHook(oSetSignonState, hkSetSignonState);
	RemoveHook(oCreateNewParticleEffect, hkCreateNewParticleEffect_proxy);
	RemoveHook(oSVCMsg_VoiceData, hkSVCMsg_VoiceData);
	RemoveHook(oDrawStaticProps, hkDrawStaticProps);
	RemoveHook(oWriteUserCmdDeltaToBuffer, hkWriteUserCmdDeltaToBuffer);
	RemoveHook(oShouldDrawViewModel, hkShouldDrawViewModel);
	RemoveHook(oPerformScreenOverlay, hkPerformScreenOverlay);
	RemoveHook(oListLeavesInBox, hkListLeavesInBox);
	RemoveHook(oInPrediction, hkInPrediction);
	RemoveHook(oCL_DispatchSound, hkCL_DispatchSound);
	RemoveHook(oInterpolateViewModel, hkInterpolateViewModel);
	RemoveHook(oThrowGrenade, hkThrowGrenade);
	RemoveHook(oCalcViewModel, hkCalcViewModel);
	RemoveHook(oSVCMsg_TempEntities, hkSVCMsg_TempEntities);
	RemoveHook(oResetLatched, hkResetLatched);
	RemoveHook(oGetExposureRange, hkGetExposureRange);
	RemoveHook(oEstimateAbsVelocity, hkEstimateAbsVelocity);
	//RemoveHook(oInterpolatePlayer, hkInterpolatePlayer);
	RemoveHook(oCIsPaused, hkCIsPaused);
}