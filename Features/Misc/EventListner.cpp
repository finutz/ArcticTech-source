#include "EventListner.h"

#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Console.h"
#include "../RageBot/Exploits.h"
#include "../RageBot/AnimationSystem.h"
#include "../RageBot/LagCompensation.h"
#include "../RageBot/Ragebot.h"
#include "../Visuals/ESP.h"
#include "../Visuals/Chams.h"
#include "../ShotManager/ShotManager.h"
#include "../Visuals/PreserveKillfeed.h"
#include "../Lua/Bridge/Bridge.h"
#include "AutoPeek.h"

CEventListner* EventListner = new CEventListner;

static std::vector<const char*> s_RgisterEvents = {
	"player_hurt",
	"player_death",
	"player_spawned",
	"player_disconnect",
	"bomb_planted",
	"bomb_defused",
	"bomb_begindefuse",
	"bomb_beginplant",
	"bomb_abortplant",
	"bomb_abortdefuse",
	"bomb_exploded",
	"round_start",
	"round_end",
	"item_purchase",
	"round_freeze_end",
	"bullet_impact",
	"item_equip"
};

void CEventListner::FireGameEvent(IGameEvent* event) {
	const std::string name = event->GetName();

	bool skip_hurt = ShotManager->OnEvent(event);

	int local_id = EngineClient->GetLocalPlayer();
	int user_id_pl = EngineClient->GetPlayerForUserID(event->GetInt("userid"));

	if (name == "player_hurt") {
		CBasePlayer* victim = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(EngineClient->GetPlayerForUserID(event->GetInt("userid"))));

		if (EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == local_id) {
			if (config.visuals.esp.hitsound->get())
				EngineClient->ExecuteClientCmd("play buttons/arena_switch_press_02.wav");

			if (!skip_hurt) {
				if (config.visuals.esp.damage_marker->get())
					ESP::AddDamageMarker(victim->m_vecOrigin() + Vector(0, 0, 80), event->GetInt("dmg_health"));
			}

			if (config.misc.miscellaneous.logs->get(0) && !skip_hurt) {
				Console->Log(std::format("hurt {}'s {} for {} damage ({} remaining)", victim->GetName(), GetHitgroupName(event->GetInt("hitgroup")), event->GetInt("dmg_health"), event->GetInt("health")));
			}
		}

		if (victim && victim->m_bDormant()) {
			ESPInfo[victim->EntIndex()].m_nHealth = event->GetInt("health");
		}
	}
	else if (name == "player_death") {
		if (user_id_pl == local_id) {
			ctx.reset();
			Exploits->target_tickbase_shift = 0;
			ctx.tickbase_shift = 0;
			AutoPeek->returning = false;
		}

		Resolver->Reset((CBasePlayer*)EntityList->GetClientEntity(user_id_pl));
		AnimationSystem->InvalidateInterpolation(user_id_pl);
		LagCompensation->Invalidate(user_id_pl);
		ESPInfo[user_id_pl].m_flLastUpdateTime = 0.f;
		ESPInfo[user_id_pl].m_nHealth = 0;
		if (PlayerResource)
			PlayerResource->m_bAlive()[user_id_pl] = false;

		Ragebot->CalcSpreadValues(); // maybe we got bad values previously?
	}
	else if (name == "round_start") {
		LagCompensation->Reset();
		ctx.should_buy = true;
		ctx.planting_bomb = false;

		//Utils::ForceFullUpdate();

		KillFeed->ClearDeathNotice = true;

		for (int i = 0; i < ClientState->m_nMaxClients; i++) {
			ESPInfo[i].m_nHealth = 100;
		}
	}
	else if (name == "bullet_impact") {
		if (user_id_pl == local_id && config.visuals.effects.server_impacts->get() && Cheat.LocalPlayer) {
			Vector pos = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
			Color col = config.visuals.effects.server_impacts_color->get();
			DebugOverlay->AddBox(pos, Vector(-1, -1, -1), Vector(1, 1, 1), col, config.visuals.effects.impacts_duration->get());
		}
	}
	else if (name == "item_equip") {
		CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(user_id_pl));

		if (player && player->m_bDormant()) {
			ESPInfo[user_id_pl].m_iActiveWeapon = event->GetInt("defindex");
		}
	}
	else if (name == "player_disconnect") {
		Chams->RemoveShotChams(user_id_pl);
		LagCompensation->Reset(user_id_pl);
		ESPInfo[user_id_pl].reset();
		AnimationSystem->InvalidateInterpolation(user_id_pl);
	}
	else if (name == "bomb_beginplant") {
		if (user_id_pl == local_id)
			ctx.planting_bomb = true;
	}
	else if (name == "bomb_abortplant") {
		if (user_id_pl == local_id)
			ctx.planting_bomb = false;
	}
	else if (name == "bomb_planted") {
		ctx.planting_bomb = false;
	}

	for (auto& func : Lua->hooks.getHooks(LUA_GAMEEVENTS))
		func.func(event);
}

int CEventListner::GetEventDebugID() {
	return 42;
}

void CEventListner::Register() {
	m_iDebugId = 42;

	for (auto name : s_RgisterEvents) {
		GameEventManager->AddListener(this, name, false);
	}
}

void CEventListner::Unregister() {
	GameEventManager->RemoveListener(this);
}