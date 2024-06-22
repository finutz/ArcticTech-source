#include "AutoStrafe.h"

#include <cmath>

#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"
#include "../Visuals/GrenadePrediction.h"

__forceinline float GetYaw(float a1, float a2) {

	if (a1 != 0.f || a2 != 0.f)
		return atan2f(a2, a1) * 57.295776f;
	return 0.f;
}

void Miscelleaneus::AutoStrafe() {
	static DWORD m_strafe_flags;
	static float m_last_yaw;

	if (!Cheat.LocalPlayer || !config.misc.movement.auto_strafe->get() || !Cheat.InGame || Cheat.LocalPlayer->m_iHealth() == 0) return;
	if (Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND || Cheat.LocalPlayer->m_MoveType() == MOVETYPE_NOCLIP || Cheat.LocalPlayer->m_MoveType() == MOVETYPE_LADDER) return;
	if (ctx.cmd->buttons & IN_SPEED) return;

	Vector velocity = Cheat.LocalPlayer->m_vecVelocity();
	float velocity_len = Cheat.LocalPlayer->m_vecVelocity().Length2D();

	float strafer_smoothing = config.misc.movement.auto_strafe_smooth->get();
	float ideal_step = min(90.f, 845.5f / velocity_len);
	float velocity_yaw = GetYaw(velocity.x, velocity.y);

	QAngle angles = ctx.cmd->viewangles;
	QAngle oangles = ctx.cmd->viewangles;

	if (velocity_len < 2.f && ctx.cmd->buttons & IN_JUMP)
		ctx.cmd->forwardmove = 450.f;

	float forward_move = ctx.cmd->forwardmove;

	if (forward_move != 0.f || ctx.cmd->sidemove != 0.f) {
		ctx.cmd->forwardmove = 0.f;

		if (velocity_len != 0.f && fabsf(velocity.z) != 0.f) {
		DO_IT_AGAIN:
			Vector forw, right, nil;
			Utils::AngleVectors(angles, forw, right, nil);

			float v262 = (forw.x * forward_move) + (ctx.cmd->sidemove * right.x);
			float v263 = (right.y * ctx.cmd->sidemove) + (forw.y * forward_move);
			angles.yaw = GetYaw(v262, v263);
		}
	}

	float yaw_to_use = 0.f;
	m_strafe_flags &= ~4;

	float clamped_angles = angles.yaw;
	if (clamped_angles < -180.f) clamped_angles += 360.f;
	if (clamped_angles > 180.f) clamped_angles -= 360.f;

	yaw_to_use = ctx.cmd->viewangles.yaw;
	m_strafe_flags |= 4;
	m_last_yaw = clamped_angles;

	if (m_strafe_flags & 4) {
		float diff = angles.yaw - yaw_to_use;
		if (diff < -180.f) diff += 360.f;
		if (diff > 180.f) diff -= 360.f;

		if (fabsf(diff) > ideal_step && fabsf(diff) <= 30.f) {
			float move = 450.f;
			if (diff < 0.f)
				move *= -1.f;

			ctx.cmd->sidemove = move;
			return;
		}
	}

	float diff = angles.yaw - velocity_yaw;
	if (diff < -180.f) diff += 360.f;
	if (diff > 180.f) diff -= 360.f;

	float step = ((100 - 30) * 0.02f) * (ideal_step + ideal_step);
	float sidemove = 0.f;
	if (fabsf(diff) > 170.f && velocity_len > 80.f || diff > step && velocity_len > 80.f) {
		angles.yaw = step + velocity_yaw;
		ctx.cmd->sidemove = -450.f;
	}
	else if (-step <= diff || velocity_len <= 80.f) {
		if (m_strafe_flags & 1) {
			angles.yaw -= ideal_step;
			ctx.cmd->sidemove = -450.f;
		}
		else {
			angles.yaw += ideal_step;
			ctx.cmd->sidemove = 450.f;
		}
	}
	else {
		angles.yaw = velocity_yaw - step;
		ctx.cmd->sidemove = 450.f;
	}
	if (!(ctx.cmd->buttons & 16) && ctx.cmd->sidemove == 0.f)
		goto DO_IT_AGAIN;

	m_strafe_flags ^= (m_strafe_flags ^ ~m_strafe_flags) & 1;

	float rotation = DEG2RAD(ctx.cmd->viewangles.yaw - angles.yaw);

	float cos_rot = std::cos(rotation);
	float sin_rot = std::sin(rotation);

	float new_forwardmove = (cos_rot * ctx.cmd->forwardmove) - (sin_rot * ctx.cmd->sidemove);
	float new_sidemove = (sin_rot * ctx.cmd->forwardmove) + (cos_rot * ctx.cmd->sidemove);

	ctx.cmd->forwardmove = new_forwardmove;
	ctx.cmd->sidemove = new_sidemove;
}

void Miscelleaneus::CompensateThrowable() {
	auto weapon = Cheat.LocalPlayer->GetActiveWeapon();

	if (!weapon->IsGrenade())
		return;

	CBaseGrenade* grenade = reinterpret_cast<CBaseGrenade*>(weapon);
	auto weaponData = weapon->GetWeaponInfo();

	if (!weaponData)
		return;

	QAngle vangle;
	EngineClient->GetViewAngles(&vangle);

	if (config.misc.movement.compensate_throwable->get(0) && config.misc.movement.auto_strafe->get()) {
		Vector direction = Math::AngleVectors(vangle);

		Vector smoothed_velocity = (ctx.local_velocity + ctx.last_local_velocity) * 0.5f;

		Vector base_vel = direction * (std::clamp(weaponData->flThrowVelocity * 0.9f, 15.f, 750.f) * (grenade->m_flThrowStrength() * 0.7f + 0.3f));
		Vector curent_vel = ctx.local_velocity * 1.25f + base_vel;

		Vector target_vel = (base_vel + smoothed_velocity * 1.25f).Normalized();
		if (curent_vel.Dot(direction) > 0.f && config.misc.movement.compensate_throwable->get(2))
			target_vel = direction;

		float throw_yaw = CalculateThrowYaw(target_vel, ctx.local_velocity, weaponData->flThrowVelocity, grenade->m_flThrowStrength());

		if (config.misc.movement.compensate_throwable->get(2) || !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND))
			ctx.cmd->viewangles.yaw = throw_yaw;
	}

	if (config.misc.movement.compensate_throwable->get(1)) {
		ctx.cmd->viewangles.pitch += CalculateThrowPitch(Math::AngleVectors(ctx.cmd->viewangles), config.misc.movement.compensate_throwable->get(2) ? 0.f : std::clamp(ctx.local_velocity.z, -120.f, 120.f), ctx.local_velocity, weaponData->flThrowVelocity, grenade->m_flThrowStrength());
	}
}