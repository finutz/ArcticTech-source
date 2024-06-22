#include "Prediction.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"

#include "../RageBot/AnimationSystem.h"
#include "../RageBot/Exploits.h"

#include "../../Utils/Console.h"

void CPrediction::StartCommand(CBasePlayer* player, CUserCmd* cmd) {
	*Cheat.LocalPlayer->GetCurrentCommand() = cmd;
	Cheat.LocalPlayer->GetLastCommand() = *cmd;
	*predictionRandomSeed = cmd->random_seed;
	*predictionEntity = Cheat.LocalPlayer;
}

void CPrediction::RunPreThink(CBasePlayer* player) {
	if (!player->PhysicsRunThink(0))
		return;

	player->PreThink();
}

void CPrediction::RunThink(CBasePlayer* player) {
	static auto SetNextThink = reinterpret_cast<void(__thiscall*)(int)>(Utils::PatternScan("client.dll", "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6"));
	int thinktick = player->m_nNextThinkTick();

	if (thinktick <= 0 || thinktick > player->m_nTickBase())
		return;

	SetNextThink(0);

	player->Think();
}

void CPrediction::BackupData() {
	if (ctx.active_weapon) {
		if (ctx.active_weapon->IsGrenade())
			m_fThrowTime = reinterpret_cast<CBaseGrenade*>(ctx.active_weapon)->m_flThrowTime();
		m_flNextPrimaryAttack = ctx.active_weapon->m_flNextPrimaryAttack();

		weaponAccuracyPenality = ctx.active_weapon->m_fAccuracyPenalty();
		weaponRecoilIndex = ctx.active_weapon->m_flRecoilIndex();
	}

	m_fFlags = Cheat.LocalPlayer->m_fFlags();
	m_vecVelocity = Cheat.LocalPlayer->m_vecVelocity();
	m_vecAbsVelocity = Cheat.LocalPlayer->m_vecAbsVelocity();

	flOldCurrentTime = GlobalVars->curtime;
	flOldFrameTime = GlobalVars->frametime;
	iOldTickcount = GlobalVars->tickcount;

	bOldIsFirstPrediction = Prediction->bIsFirstTimePredicted;
	bOldInPrediction = Prediction->bInPrediction;
}

void CPrediction::Start(CUserCmd* cmd) {
	if (!MoveHelper)
		return;

	Prediction->Update(ClientState->m_nDeltaTick, ClientState->m_nDeltaTick > 0, ClientState->m_nLastCommandAck, ClientState->m_nLastOutgoingCommand + ClientState->m_nChokedCommands);

	local_data[cmd->command_number % MULTIPLAYER_BACKUP].init(cmd);

	BackupData();
	StartCommand(Cheat.LocalPlayer, cmd);

	const float backup_velocity_modifier = Cheat.LocalPlayer->m_flVelocityModifier();

	GlobalVars->curtime = TICKS_TO_TIME(Cheat.LocalPlayer->m_nTickBase());
	GlobalVars->frametime = Prediction->bEnginePaused ? 0.f : GlobalVars->interval_per_tick;

	Prediction->bIsFirstTimePredicted = false;
	Prediction->bInPrediction = true;

	GameMovement->StartTrackPredictionErrors(Cheat.LocalPlayer);

	Cheat.LocalPlayer->UpdateButtonState(cmd->buttons);
	memset(&moveData, 0, sizeof(moveData));

	Prediction->CheckMovingGround(Cheat.LocalPlayer, GlobalVars->frametime);
	Prediction->SetLocalViewAngles(cmd->viewangles);

	RunPreThink(Cheat.LocalPlayer);
	RunThink(Cheat.LocalPlayer);

	MoveHelper->SetHost(Cheat.LocalPlayer);
	Prediction->SetupMove(Cheat.LocalPlayer, cmd, MoveHelper, &moveData);
	GameMovement->ProcessMovement(Cheat.LocalPlayer, &moveData);
	Prediction->FinishMove(Cheat.LocalPlayer, cmd, &moveData);

	MoveHelper->ProcessImpacts();

	Cheat.LocalPlayer->PostThink();

	Cheat.LocalPlayer->m_flVelocityModifier() = backup_velocity_modifier;

	if (ctx.active_weapon) {
		ctx.active_weapon->UpdateAccuracyPenality();
		weaponInaccuracy = ctx.active_weapon->GetInaccuracy();
		weaponSpread = ctx.active_weapon->GetSpread();
	}

	AnimationSystem->UpdatePredictionAnimation();
	ctx.shoot_position = Cheat.LocalPlayer->GetShootPosition();
}

void CPrediction::End() {
	if (!MoveHelper)
		return;

	GlobalVars->curtime = flOldCurrentTime;
	GlobalVars->frametime = flOldFrameTime;
	GlobalVars->tickcount = iOldTickcount;

	*Cheat.LocalPlayer->GetCurrentCommand() = nullptr;
	*predictionRandomSeed = -1;
	*predictionEntity = nullptr;

	GameMovement->FinishTrackPredictionErrors(Cheat.LocalPlayer);
	MoveHelper->SetHost(nullptr);
	
	if (ctx.active_weapon) {
		ctx.active_weapon->m_fAccuracyPenalty() = weaponAccuracyPenality;
		ctx.active_weapon->m_flRecoilIndex() = weaponRecoilIndex;
	}

	GameMovement->Reset();

	Prediction->bInPrediction = bOldInPrediction;
	Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void CPrediction::PatchAttackPacket(CUserCmd* cmd, bool restore)
{
	static bool m_bLastAttack = false;
	static bool m_bInvalidCycle = false;
	static float m_flLastCycle = 0.f;

	if (!Cheat.LocalPlayer)
		return;

	if (restore)
	{
		m_bLastAttack = cmd->weaponselect || (cmd->buttons & IN_ATTACK);
		m_flLastCycle = Cheat.LocalPlayer->m_flCycle();
	}
	else if (m_bLastAttack && !m_bInvalidCycle)
		m_bInvalidCycle = Cheat.LocalPlayer->m_flCycle() == 0.f && m_flLastCycle > 0.f;

	if (m_bInvalidCycle)
		Cheat.LocalPlayer->m_flCycle() = m_flLastCycle;
}

void CPrediction::StoreNetvars(int place) {
	CBasePlayer* local = Cheat.LocalPlayer;

	auto& nv = local_netvars[place % MULTIPLAYER_BACKUP];

	nv.command_number = place;
	nv.m_nTickBase = local->m_nTickBase();

	nv.m_aimPunchAngle = local->m_aimPunchAngle();
	nv.m_aimPunchAngleVel = local->m_aimPunchAngleVel();
	nv.m_flDuckAmount = local->m_flDuckAmount();
	nv.m_flFallVelocity = local->m_flFallVelocity();
	nv.m_vecViewOffset = local->m_vecViewOffset();
	nv.m_viewPunchAngle = local->m_viewPunchAngle();
	nv.m_vecVelocity = local->m_vecVelocity();
	nv.m_nTickBase = local->m_nTickBase();
	nv.m_flDuckSpeed = local->m_flDuckSpeed();
	nv.m_flVelocityModifier = local->m_flVelocityModifier();
	nv.m_flThirdpersonRecoil = local->m_flThirdpersonRecoil();
	nv.filled = true;
}

void CPrediction::RestoreNetvars(int place) {
	CBasePlayer* local = Cheat.LocalPlayer;

	auto& nv = local_netvars[place % MULTIPLAYER_BACKUP];

	has_prediction_errors = false;

	if (!nv.filled || nv.command_number != place)
		return;

	auto aim_punch_vel_diff = nv.m_aimPunchAngleVel - Cheat.LocalPlayer->m_aimPunchAngleVel();
	auto aim_punch_diff = nv.m_aimPunchAngle - Cheat.LocalPlayer->m_aimPunchAngle();
	auto viewoffset_diff = nv.m_vecViewOffset - Cheat.LocalPlayer->m_vecViewOffset();
	auto velocity_diff = nv.m_vecVelocity - Cheat.LocalPlayer->m_vecVelocity();
	auto fall_vel_diff = nv.m_flFallVelocity - Cheat.LocalPlayer->m_flFallVelocity();
	auto net_origin_diff = nv.m_vecNetworkOrigin - Cheat.LocalPlayer->m_vecNetworkOrigin();

	if (std::abs(aim_punch_diff.pitch) <= 0.03125f && std::abs(aim_punch_diff.yaw) <= 0.03125 && std::abs(aim_punch_diff.roll) <= 0.03125f)
		Cheat.LocalPlayer->m_aimPunchAngle() = nv.m_aimPunchAngle;
	else
		has_prediction_errors = true;

	if (std::abs(aim_punch_vel_diff.pitch) <= 0.03125f && std::abs(aim_punch_vel_diff.yaw) <= 0.03125 && std::abs(aim_punch_vel_diff.roll) <= 0.03125f)
		Cheat.LocalPlayer->m_aimPunchAngleVel() = nv.m_aimPunchAngleVel;
	else
		has_prediction_errors = true;

	if (std::abs(viewoffset_diff.z) <= 0.065f)
		Cheat.LocalPlayer->m_vecViewOffset() = nv.m_vecViewOffset;
	else
		has_prediction_errors = true;

	if (std::abs(fall_vel_diff) <= 0.5f)
		Cheat.LocalPlayer->m_flFallVelocity() = nv.m_flFallVelocity;
	else
		has_prediction_errors = true;

	if (std::abs(velocity_diff.x) > 0.5f || std::abs(velocity_diff.y) > 0.5f || std::abs(velocity_diff.z) > 0.5f)
		has_prediction_errors = true;

	if (std::abs(net_origin_diff.x) > 0.0625f || std::abs(net_origin_diff.y) > 0.0625f || std::abs(net_origin_diff.z) > 0.0625f)
		has_prediction_errors = true;
}

CPrediction* EnginePrediction = new CPrediction;