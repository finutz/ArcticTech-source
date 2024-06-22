#include "AntiAim.h"
#include "../RageBot/Exploits.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include <algorithm>
#include "../Misc/Prediction.h"
#include "../RageBot/AutoWall.h"
#include "../../Utils/Console.h"
#include "../RageBot/AnimationSystem.h"
#include "../Lua/Bridge/Bridge.h"
#include "../Visuals/ESP.h"

void CAntiAim::FakeLag() {
	static ConVar* sv_maxusrcmdprocessticks = CVar->FindVar("sv_maxusrcmdprocessticks");

	if (Cheat.LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP || Cheat.LocalPlayer->m_fFlags() & FL_FROZEN || GameRules()->IsFreezePeriod())
		return;

	if (!ctx.active_weapon)
		return;

	if (ctx.active_weapon->IsGrenade() && (EnginePrediction->m_fThrowTime > 0 || reinterpret_cast<CBaseGrenade*>(ctx.active_weapon)->m_flThrowTime() > 0.f || abs(ctx.grenade_throw_tick - ctx.cmd->command_number) < 7)) {
		ctx.send_packet = true;
		return;
	}
	else if (ctx.cmd->buttons & IN_ATTACK && ctx.active_weapon->ShootingWeapon() && ctx.active_weapon->CanShoot()) {
		if (!config.antiaim.misc.fake_duck->get()) {
			ctx.send_packet = true;
			return;
		}
	}

	if (Exploits->GetExploitType() == CExploits::E_DoubleTap && ctx.tickbase_shift && ctx.cmd->buttons & IN_USE)
		return;

	fakelag = 0;
	fakelag_limit = min(sv_maxusrcmdprocessticks->GetInt(), config.antiaim.fakelag.limit->get());

	if (ctx.tickbase_shift > 0) {
		fakelag_limit = max((sv_maxusrcmdprocessticks->GetInt() - 2) - ctx.tickbase_shift, 1);
	}

	if (config.ragebot.aimbot.doubletap->get() && (GlobalVars->realtime - ctx.last_shot_time) < 0.5f) {
		fakelag_limit = 2;
	}

	if (config.antiaim.fakelag.enabled->get()) {
		if (Cheat.LocalPlayer->m_vecVelocity().LengthSqr() < 4096.f) {
			fakelag = 2;
		}
		else {
			fakelag = fakelag_limit;

			if (!(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND)) {
				fakelag = int(64.0f / (Cheat.LocalPlayer->m_vecVelocity().Q_Length() * GlobalVars->interval_per_tick) + 1);
			} else {
				if (config.antiaim.fakelag.variability->get() > 0)
					fakelag -= Utils::RandomInt(0, config.antiaim.fakelag.variability->get());
			}
		}
	}

	if (lua_override.override_bits & LuaAntiAim_t::OverrideFakeLag)
		fakelag = lua_override.fakelag;

	if (config.antiaim.misc.fake_duck->get()) {
		fakelag = 14;
		fakelag_limit = 14;
	}

	fakelag = std::clamp(fakelag, 0, fakelag_limit);
	
	ctx.send_packet = ClientState->m_nChokedCommands >= fakelag;

	static bool hasPeeked = false;

	if (ctx.is_peeking && !ctx.fake_duck) {
		if (!hasPeeked) {
			hasPeeked = true;

			if (ClientState->m_nChokedCommands > 0 && ctx.tickbase_shift == 0)
				ctx.send_packet = true;
		}
	}
	else {
		hasPeeked = false;
	}
}

void CAntiAim::Angles() {
	desyncing = false;

	if (GameRules()->IsFreezePeriod() || Cheat.LocalPlayer->m_fFlags() & FL_FROZEN || (Cheat.LocalPlayer->m_MoveType() == MOVETYPE_LADDER && ctx.cmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK)) || Cheat.LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	if (!ctx.active_weapon)
		return;

	if (ctx.active_weapon->IsGrenade() && (EnginePrediction->m_fThrowTime > 0 || reinterpret_cast<CBaseGrenade*>(ctx.active_weapon)->m_flThrowTime() > 0.f || abs(ctx.grenade_throw_tick - ctx.cmd->command_number) < 7))
		return;

	if (ctx.active_weapon->ShootingWeapon() && ctx.active_weapon->CanShoot() && ctx.cmd->buttons & IN_ATTACK)
		return;

	target = GetNearestTarget();
	pitch = ctx.cmd->viewangles.pitch;
	base_yaw = ctx.cmd->viewangles.yaw;

	if (!(ctx.cmd->buttons & IN_USE) || ctx.planting_bomb) {
		switch (config.antiaim.angles.pitch->get()) {
		case 0:
			break;
		case 1:
			pitch = 89;
			break;
		}

		float originalYaw = ctx.cmd->viewangles.yaw;

		switch (config.antiaim.angles.yaw->get()) {
		case 0:
			break;
		case 1:
			base_yaw -= 180;
			break;
		case 2:
			base_yaw = AtTargets();
			break;
		}

		if (manualAngleState) {
			base_yaw = originalYaw + ((manualAngleState == 1) ? 90 : -90);
		}

		if (lua_override.override_bits & LuaAntiAim_t::OverrideYaw)
			base_yaw = lua_override.yaw;

		yaw_offset = 0;
		if (config.antiaim.angles.yaw_jitter->get() && !Cheat.LocalPlayer->m_bIsDefusing() && (!config.antiaim.angles.manual_options->get(0) || manualAngleState == 0))
			yaw_offset = jitter ? -config.antiaim.angles.modifier_value->get() * 0.5f : config.antiaim.angles.modifier_value->get() * 0.5f;

		if (lua_override.override_bits & lua_override.OverridePitch)
			pitch = lua_override.pitch;
		if (lua_override.override_bits & lua_override.OverrideYaw)
			base_yaw = lua_override.yaw;
		if (lua_override.override_bits & lua_override.OverrideYawOffset)
			yaw_offset = lua_override.yaw_offset;

		ctx.cmd->viewangles.yaw = base_yaw + yaw_offset;
		ctx.cmd->viewangles.pitch = pitch;
	}

	Desync();
}

void CAntiAim::Desync() {
	if (!config.antiaim.angles.body_yaw->get() || 
		Exploits->IsShifting() || 
		Cheat.LocalPlayer->m_bIsDefusing() || 
		(ctx.cmd->buttons & IN_USE && !(lua_override.override_bits & lua_override.OverrideDesync) && !lua_override.desync) ||
		((lua_override.override_bits & lua_override.OverrideDesync) && !lua_override.desync))
		return;

	bool inverter = config.antiaim.angles.inverter->get();

	if (config.antiaim.angles.body_yaw_options->get(0) && (!config.antiaim.angles.manual_options->get(0) || manualAngleState == 0))
		inverter = jitter;

	if (config.antiaim.angles.body_yaw_options->get(3) || (manualAngleState != 0 && config.antiaim.angles.manual_options->get(1))) {
		int fs_side = DesyncFreestand();

		if (fs_side != 0)
			inverter = fs_side == 1;
	}

	if (lua_override.override_bits & lua_override.OverrideDesyncSide)
		inverter = lua_override.desync_side == 1;

	desync_limit = config.antiaim.angles.body_yaw_limit->get() * 2.f;

	if (lua_override.override_bits & lua_override.OverrideDesyncAngle)
		desync_limit = lua_override.desync_angle;

	float desync_angle = desync_limit * (inverter ? 1 : -1);

	if (!ctx.send_packet) {
		float max_desync_angle = Cheat.LocalPlayer->GetMaxDesyncDelta();
		realAngle = Math::AngleNormalize(ctx.cmd->viewangles.yaw + std::clamp(desync_angle, -max_desync_angle, max_desync_angle));

		ctx.cmd->viewangles.yaw += desync_angle;
	}

	if (config.antiaim.angles.body_yaw_options->get(2) && ctx.send_packet && (!ctx.tickbase_shift || ctx.cmd->buttons & IN_DUCK || config.antiaim.misc.slow_walk->get() || !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND && Cheat.LocalPlayer->m_vecVelocity().LengthSqr() > 400.f))) {
		ctx.cmd->viewangles.roll = desync_angle < 0 ? 64: -64;
	}

	desyncing = true;

	if (ctx.local_velocity.LengthSqr() < 5.f) {
		ctx.cmd->sidemove = ctx.cmd->buttons & IN_DUCK ? (ctx.cmd->tick_count % 2 == 0 ? -3.3f : 3.3f) : (ctx.cmd->tick_count % 2 == 0 ? -1.1f : 1.1f);
	}
}

CBasePlayer* CAntiAim::GetNearestTarget() {
	float nearestFov = 360.f;
	Vector eyePos = Cheat.LocalPlayer->GetEyePosition();
	QAngle viewAngle; EngineClient->GetViewAngles(&viewAngle);
	CBasePlayer* nearestPlayer = nullptr;

	for (int i = 0; i < ClientState->m_nMaxClients; i++) {
		CBasePlayer* pl = (CBasePlayer*)EntityList->GetClientEntity(i);

		if (!pl)
			continue;

		if (!pl->IsPlayer() || pl->IsTeammate() || !pl->IsAlive())
			continue;

		if (EnginePrediction->curtime() - ESPInfo[i].m_flLastUpdateTime > 10.f) // old dormant
			continue;

		QAngle angleToPlayer = Math::VectorAngles(pl->m_vecOrigin() - eyePos);

		float fov = Utils::GetFOV(viewAngle, angleToPlayer);

		if (!pl->m_bDormant()) {
			CTraceFilterWorldAndPropsOnly filter;
			CGameTrace trace;
			Vector v1 = pl->GetEyePosition();
			Vector v2 = Cheat.LocalPlayer->GetEyePosition();
			if (std::abs(v1.z - v2.z) < 0.01f)
				v2.z -= 1.f;

			Ray_t ray(v1, v2);

			EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);

			if (trace.fraction == 1.f)
				fov /= 180.f;
		}

		if (fov < nearestFov) {
			nearestFov = fov;
			nearestPlayer = pl;
		}
	}

	return nearestPlayer;
}

float CAntiAim::AtTargets() {
	if (!target)
		return ctx.cmd->viewangles.yaw - 180.f;

	QAngle backwardAngle = Math::VectorAngles(Cheat.LocalPlayer->m_vecOrigin() - target->m_vecOrigin());
	return backwardAngle.yaw;
}

int CAntiAim::DesyncFreestand() {
	if (!target)
		return 0;

	Vector forward = (target->m_vecOrigin() - Cheat.LocalPlayer->m_vecOrigin()).Q_Normalized();
	Vector eyePos = Cheat.LocalPlayer->GetEyePosition();

	Vector right = Math::AngleVectors(QAngle(0, base_yaw + 90.f, 0));

	Vector negPos = eyePos - right * 16.f;
	Vector posPos = eyePos + right * 16.f;

	CGameTrace negTrace = EngineTrace->TraceRay(negPos, negPos + forward * 100.f, MASK_SHOT_HULL | CONTENTS_GRATE, Cheat.LocalPlayer);
	CGameTrace posTrace = EngineTrace->TraceRay(posPos, posPos + forward * 100.f, MASK_SHOT_HULL | CONTENTS_GRATE, Cheat.LocalPlayer);

	if (negTrace.startsolid && posTrace.startsolid)
		return 0;
	else if (negTrace.startsolid)
		return -1;
	else if (posTrace.startsolid)
		return 1;

	if (negTrace.fraction == 1.f && posTrace.fraction == 1.f)
		return 0;

	return negTrace.fraction < posTrace.fraction ? -1 : 1;
}

void CAntiAim::SlowWalk() {
	if (!config.antiaim.misc.slow_walk->get() || !Cheat.LocalPlayer || Cheat.LocalPlayer->m_iHealth() == 0 || !(GetAsyncKeyState(VK_SHIFT) & 0x8000))
		return;

	ctx.cmd->buttons &= ~IN_WALK;

	if (!ctx.active_weapon)
		return;

	float maxSpeed = (Cheat.LocalPlayer->m_bIsScoped() ? ctx.weapon_info->flMaxSpeedAlt : ctx.weapon_info->flMaxSpeed) * 0.3f;

	float movespeed = Math::Q_sqrt(ctx.cmd->sidemove * ctx.cmd->sidemove + ctx.cmd->forwardmove * ctx.cmd->forwardmove);
	
	if (movespeed == 0)
		return;

	float modifier = maxSpeed / movespeed;

	ctx.cmd->sidemove = min(modifier * ctx.cmd->sidemove, 450);
	ctx.cmd->forwardmove = min(modifier * ctx.cmd->forwardmove, 450);
}

void CAntiAim::FakeDuck() {
	ctx.fake_duck = false;

	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND) || !config.antiaim.misc.fake_duck->get())
		return;

	ctx.cmd->buttons |= IN_BULLRUSH;

	if (ClientState->m_nChokedCommands < 7 && !(Exploits->IsShifting() || (GlobalVars->realtime - Exploits->last_teleport_time) < TICKS_TO_TIME(14)))
		ctx.cmd->buttons &= ~IN_DUCK;
	else
		ctx.cmd->buttons |= IN_DUCK;

	ctx.fake_duck = true;
}

void CAntiAim::LegMovement() {
	if (!Cheat.LocalPlayer || Cheat.LocalPlayer->m_MoveType() == MOVETYPE_LADDER)
		return;

	ctx.cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);

	int target_type = config.antiaim.misc.leg_movement->get();
	if (target_type == 1 && ctx.send_packet && !(ctx.cmd->buttons & IN_USE && ctx.tickbase_shift == 13)) {
		target_type = 2;
	}
	else if (target_type == 1) {
		ctx.leg_slide_angle = ctx.cmd->viewangles;
	}

	switch (target_type) {
	case 0:
		if (ctx.cmd->forwardmove != 0)
			ctx.cmd->buttons |= ctx.cmd->forwardmove > 0 ? IN_FORWARD : IN_BACK;
		if (ctx.cmd->sidemove != 0)
			ctx.cmd->buttons |= ctx.cmd->sidemove > 0 ? IN_MOVERIGHT : IN_MOVELEFT;
		break;
	case 1:
		if (ctx.cmd->forwardmove != 0)
			ctx.cmd->buttons |= ctx.cmd->forwardmove < 0 ? IN_FORWARD : IN_BACK;
		if (ctx.cmd->sidemove != 0)
			ctx.cmd->buttons |= ctx.cmd->sidemove < 0 ? IN_MOVERIGHT : IN_MOVELEFT;
		break;
	default:
		break;
	}
}

void CAntiAim::JitterMove() {
	if (!config.antiaim.misc.jitter_move->get())
		return;

	if (!Cheat.LocalPlayer || !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	if (ctx.active_weapon && ctx.active_weapon->IsGrenade())
		return;

	if (abs(ctx.cmd->sidemove) + abs(ctx.cmd->forwardmove) < 10.f)
		return;

	if (Cheat.LocalPlayer->m_vecVelocity().LengthSqr() < 140.f * 140.f)
		return;

	float factor = 0.95f + fmodf(GlobalVars->curtime, 0.2f) * 0.25f;

	ctx.cmd->sidemove = std::clamp(ctx.cmd->sidemove, -250.f, 250.f) * factor;
	ctx.cmd->forwardmove = std::clamp(ctx.cmd->forwardmove, -250.f, 250.f) * factor;
}

bool CAntiAim::IsPeeking() {
	if (Exploits->GetExploitType() == CExploits::E_HideShots)
		return false;

	Vector velocity = Cheat.LocalPlayer->m_vecVelocity();

	if (velocity.LengthSqr() < 64.f)
		return false;

	Vector move_factor = velocity.Normalized() * 9.4f + (velocity * TICKS_TO_TIME(3.4f));
	
	Vector backup_abs_orgin = Cheat.LocalPlayer->GetAbsOrigin();
	Vector backup_origin = Cheat.LocalPlayer->m_vecOrigin();

	memcpy(Cheat.LocalPlayer->GetCachedBoneData().Base(), AnimationSystem->GetLocalBoneMatrix(), sizeof(matrix3x4_t) * Cheat.LocalPlayer->GetCachedBoneData().Count());
	Utils::MatrixMove(Cheat.LocalPlayer->GetCachedBoneData().Base(), Cheat.LocalPlayer->GetCachedBoneData().Count(), AnimationSystem->GetLocalSentAbsOrigin(), Cheat.LocalPlayer->GetAbsOrigin() + move_factor);
	Cheat.LocalPlayer->SetAbsOrigin(backup_abs_orgin + move_factor);
	Cheat.LocalPlayer->m_vecOrigin() += move_factor;
	Cheat.LocalPlayer->ForceBoneCache();

	Vector scan_points[] = {
		Cheat.LocalPlayer->GetHitboxCenter(HITBOX_HEAD),
		Cheat.LocalPlayer->GetHitboxCenter(HITBOX_PELVIS),
		Cheat.LocalPlayer->GetHitboxCenter(HITBOX_LEFT_CALF),
		Cheat.LocalPlayer->GetHitboxCenter(HITBOX_RIGHT_CALF)
	};

	auto backup_active_weapon = ctx.active_weapon;
	auto backup_weapon_data = ctx.weapon_info;

	bool peeked = false;
	for (int i = 0; i < ClientState->m_nMaxClients; i++) {
		CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(i));

		if (!player || player->IsTeammate() || !player->IsAlive() || player->m_bDormant())
			continue;

		Vector target_vel = player->m_vecVelocity();

		Vector enemyShootPos = player->GetShootPosition() + target_vel * GlobalVars->interval_per_tick;
		ctx.active_weapon = player->GetActiveWeapon();

		if (ctx.active_weapon)
			ctx.weapon_info = ctx.active_weapon->GetWeaponInfo();

		for (int i = 0; i < 4; i++) {
			FireBulletData_t data;
			if (AutoWall->FireBullet(player, enemyShootPos, scan_points[i], data, Cheat.LocalPlayer) && data.damage > 1.f) {
				peeked = true;
				break;
			}
		}

		if (peeked)
			break;
	}

	//int r = peeked ? 0 : 255;
	//int g = peeked ? 255 : 0;

	//DebugOverlay->AddBoxOverlay(scan_points[0], Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(), r, g, 0, 255, GlobalVars->interval_per_tick * 2);
	//DebugOverlay->AddBoxOverlay(scan_points[1], Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(), r, g, 0, 255, GlobalVars->interval_per_tick * 2);
	//DebugOverlay->AddBoxOverlay(scan_points[2], Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(), r, g, 0, 255, GlobalVars->interval_per_tick * 2);
	//DebugOverlay->AddBoxOverlay(scan_points[3], Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(), r, g, 0, 255, GlobalVars->interval_per_tick * 2);


	ctx.active_weapon = backup_active_weapon;
	ctx.weapon_info = backup_weapon_data;

	Cheat.LocalPlayer->SetAbsOrigin(backup_abs_orgin);
	Cheat.LocalPlayer->m_vecOrigin() = backup_origin;

	return peeked;
}

void CAntiAim::OnKeyPressed(WPARAM key) {
	if (key == config.antiaim.angles.manual_left->key) {
		if (manualAngleState == 0 || manualAngleState == 2)
			manualAngleState = 1;
		else if (manualAngleState == 1)
			manualAngleState = 0;
	}
	else if (key == config.antiaim.angles.manual_right->key) {
		if (manualAngleState == 0 || manualAngleState == 1)
			manualAngleState = 2;
		else if (manualAngleState == 2)
			manualAngleState = 0;
	}
}

CAntiAim* AntiAim = new CAntiAim();