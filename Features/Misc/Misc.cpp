#include "Misc.h"
#include "../RageBot/Exploits.h"
#include "Prediction.h"
#include "../../Utils/Utils.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Config.h"
#include "../../SDK/Globals.h"

void Miscelleaneus::Clantag()
{
	static auto removed = false;

	if (config.misc.miscellaneous.clantag->get())
	{
		auto nci = EngineClient->GetNetChannelInfo();

		if (!nci)
			return;

		static auto time = -1;

		auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)GlobalVars->tickcount; //-V807
		auto intervals = 0.3f / GlobalVars->interval_per_tick;

		auto main_time = (int)(ticks / intervals) % 28;

		if (main_time != time && !ClientState->m_nChokedCommands)
		{
			auto tag = ("");

			switch (main_time)
			{
			case 0: tag = (""); break;
			case 1: tag = ("a"); break;
			case 2: tag = ("ar"); break;
			case 3: tag = ("arc");break;
			case 4: tag = ("arct");break;
			case 5: tag = ("arcti");break;
			case 6: tag = ("arctic");break;
			case 7: tag = ("arctict");break;
			case 8: tag = ("arcticte");break;
			case 9: tag = ("arctictec");break;
			case 10: tag = ("arctictech");break;
			case 11: tag = ("arctictech");break;
			case 12: tag = ("arctictech");break;
			case 13: tag = ("arctictech");break;
			case 14: tag = ("arctictec");break;
			case 15: tag = ("arcticte");break;
			case 16: tag = ("arctict");break;
			case 17: tag = ("arctic");break;
			case 18: tag = ("arcti");break;
			case 19: tag = ("arct");break;
			case 20: tag = ("arc");break;
			case 21: tag = ("ar");break;
			case 22: tag = ("a");break;
			case 23: tag = ("");break;
			}

			Utils::SetClantag(tag);
			time = main_time;
		}

		removed = false;
	}
}

void Miscelleaneus::FastThrow() {
	static bool fast_throw_triggred = false;
	static bool quick_switch_triggered = false;

	if (!config.ragebot.aimbot.doubletap_options->get(3) || !ctx.active_weapon || !ctx.active_weapon->IsGrenade()) {
		fast_throw_triggred = false;
		quick_switch_triggered = false;
		return;
	}

	CBaseGrenade* grenade = reinterpret_cast<CBaseGrenade*>(ctx.active_weapon);

	float next_attack = max(Cheat.LocalPlayer->m_flNextAttack(), grenade->m_flNextPrimaryAttack());

	if (Exploits->GetExploitType() == CExploits::E_DoubleTap) {
		Exploits->LC_OverrideTickbase(13);
		if (next_attack - TICKS_TO_TIME(Cheat.LocalPlayer->m_nTickBase()) < TICKS_TO_TIME(4))
			quick_switch_triggered = true;

		if (quick_switch_triggered)
			Exploits->LC_OverrideTickbase(9);
		if (fast_throw_triggred)
			Exploits->LC_OverrideTickbase(0);

		if (EnginePrediction->m_fThrowTime > 0.f)
			fast_throw_triggred = true;
	}

	if (ctx.grenade_throw_tick + 7 == ctx.cmd->command_number) {
		CBaseCombatWeapon* best_weapon = nullptr;
		auto weapons = Cheat.LocalPlayer->m_hMyWeapons();
		int best_type = WEAPONTYPE_KNIFE;
		for (int i = 0; i < MAX_WEAPONS; i++) {
			auto weap = weapons[i];
			if (weap == INVALID_EHANDLE_INDEX)
				break;

			CBaseCombatWeapon* weapon = reinterpret_cast<CBaseCombatWeapon*>(EntityList->GetClientEntityFromHandle(weap));

			if (!weapon)
				continue;

			CCSWeaponData* weap_info = weapon->GetWeaponInfo();

			if (!weap_info)
				continue;

			if (weap_info->nWeaponType >= WEAPONTYPE_SUBMACHINEGUN && weap_info->nWeaponType <= WEAPONTYPE_MACHINEGUN) {
				best_weapon = weapon;
				best_type = weap_info->nWeaponType;
			}
			else if (weap_info->nWeaponType == WEAPONTYPE_PISTOL && best_type == WEAPONTYPE_KNIFE) {
				best_weapon = weapon;
				best_type = weap_info->nWeaponType;
			}
		}

		if (best_weapon)
			ctx.cmd->weaponselect = best_weapon->EntIndex();
	}
}

void Miscelleaneus::AutomaticGrenadeRelease() {
	static bool prev_release = false;
	static Vector on_release_move;
	static QAngle on_release_angle;

	if (ctx.should_release_grenade && ctx.active_weapon && ctx.active_weapon->IsGrenade()) {
		if (!prev_release) {
			on_release_move = Vector(ctx.cmd->sidemove, ctx.cmd->forwardmove);
			on_release_angle = ctx.cmd->viewangles;
		}

		ctx.cmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);

		ctx.cmd->sidemove = on_release_move.x;
		ctx.cmd->forwardmove = on_release_move.y;
		ctx.cmd->viewangles = on_release_angle;
	}
	else if (!ctx.active_weapon || !ctx.active_weapon->IsGrenade()) {
		ctx.should_release_grenade = false;
	}

	prev_release = ctx.should_release_grenade;
}