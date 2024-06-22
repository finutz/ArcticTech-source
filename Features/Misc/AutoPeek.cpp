#include "AutoPeek.h"
#include "Prediction.h"
#include "../../SDK/Globals.h"
#include "../../SDK/Render.h"

CAutoPeek* AutoPeek = new CAutoPeek;


void CAutoPeek::Draw() {
	static bool prev_state = false;

	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || !Cheat.InGame)
		return;

	bool state = config.ragebot.aimbot.peek_assist->get() && config.ragebot.aimbot.peek_assist_keybind->get();

	if (prev_state != state) {
		if (state) {
			if (Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND) {
				position = Cheat.LocalPlayer->m_vecOrigin();
			}
			else {
				CGameTrace trace = EngineTrace->TraceHull(Cheat.LocalPlayer->m_vecOrigin(), Cheat.LocalPlayer->m_vecOrigin() - Vector(0, 0, 128), Cheat.LocalPlayer->m_vecMins(), Cheat.LocalPlayer->m_vecMaxs(), CONTENTS_SOLID, Cheat.LocalPlayer);

				position = trace.endpos;
			}
		}

		prev_state = state;
	}

	circleAnimation.UpdateAnimation(state);

	float alpha = circleAnimation.GetValue();

	if (alpha > 0.01f)
		Render->Circle3DGradient(position, 22.f, config.ragebot.aimbot.peek_assist_color->get().alpha_modulatef(alpha));
}

void CAutoPeek::CreateMove() {
	if (!(config.ragebot.aimbot.peek_assist_keybind->get() && config.ragebot.aimbot.peek_assist->get()) || !ctx.active_weapon) {
		returning = false;
		return;
	}

	float distance_sqr = (Cheat.LocalPlayer->m_vecOrigin() - position).LengthSqr();

	if (ctx.cmd->buttons & IN_ATTACK && ctx.active_weapon->ShootingWeapon() && ctx.active_weapon->CanShoot()) {
		returning = true;
	} else if (distance_sqr < 4.f) {
		if (ctx.active_weapon->m_flNextPrimaryAttack() - 0.15f < TICKS_TO_TIME(Cheat.LocalPlayer->m_nTickBase()))
			returning = false;
	}

	if (returning) {
		QAngle ang = Utils::VectorToAngle(Cheat.LocalPlayer->m_vecOrigin(), position);
		QAngle vang;
		EngineClient->GetViewAngles(&vang);
		ang.yaw = vang.yaw - ang.yaw;
		ang.Normalize();

		float vel = (distance_sqr > 25.f) ? 450 : (5.f + 3.f * distance_sqr);
		if (distance_sqr < 1.f)
			vel = 0.f;

		Vector dir;
		Utils::AngleVectors(ang, dir);
		dir *= vel;
		ctx.cmd->forwardmove = dir.x;
		ctx.cmd->sidemove = dir.y;
	}
}