#include "ShotManager.h"

#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Console.h"

#include "../RageBot/AutoWall.h"
#include "../RageBot/LagCompensation.h"
#include "../Visuals/ESP.h"

#include "../Lua/Bridge/Bridge.h"


CShotManager* ShotManager = new CShotManager;

void CShotManager::DetectUnregisteredShots() {
	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive())
		return;

	CBaseCombatWeapon* weapon = Cheat.LocalPlayer->GetActiveWeapon();

	if (!weapon)
		return;

	float fShotTime = weapon->m_fLastShotTime();

	if (weapon != last_weapon) {
		last_weapon = weapon;
		m_fLastShotTime = fShotTime;
	}

	if (m_fLastShotTime - fShotTime < TICKS_TO_TIME(3.f) || (weapon->m_iItemDefinitionIndex() != Ssg08 && weapon->m_iItemDefinitionIndex() != Awp)){
		m_fLastShotTime = fShotTime;
		return;
	}

	for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
		if (it->acked)
			break;

		if (it->ack_tick != 0)
			continue;

		it->acked = true;
		it->ack_tick = GlobalVars->tickcount;
		it->unregistered = true;
		it->miss_reason = "unregistered shot";

		Console->ArcticTag();
		Console->ColorPrint("missed shot due to ", Color(230, 230, 230));
		Console->ColorPrint("unregistered shot\n", Color(255, 20, 20));

		for (auto& callback : Lua->hooks.getHooks(LUA_AIM_ACK))
			callback.func(*it);

		weapon->m_flNextPrimaryAttack() = TICKS_TO_TIME(it->shot_tick) - 0.2f; // so we can shoot again immediately
		ctx.was_unregistered_shot = true;

		break;
	}

	m_fLastShotTime = fShotTime;
}

void CShotManager::ProcessManualShot() {
	if (!config.visuals.effects.client_impacts->get())
		return;

	FireBulletData_t bullet;
	AutoWall->FireBullet(Cheat.LocalPlayer, ctx.shoot_position, ctx.shoot_position + Math::AngleVectors(ctx.cmd->viewangles) * 8192.f, bullet);

	Color col = config.visuals.effects.client_impacts_color->get();

	for (int i = 0; i < bullet.num_impacts; i++)
		DebugOverlay->AddBox(bullet.impacts[i], Vector(-1, -1, -1), Vector(1, 1, 1), col, config.visuals.effects.impacts_duration->get());
}

bool CShotManager::OnEvent(IGameEvent* event) {
	const std::string event_name(event->GetName());

	if (event_name == "player_hurt") {
		const int attacker = EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
		CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(EngineClient->GetPlayerForUserID(event->GetInt("userid"))));

		if (attacker != EngineClient->GetLocalPlayer())
			return false;

		RegisteredShot_t* shot = nullptr;

		for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
			if (it->acked)
				break;

			if (it->record->player == player)
				shot = &*it;
		}

		if (!shot)
			return false; // manual fire or miss

		shot->ack_tick = GlobalVars->tickcount;
		shot->damage = event->GetInt("dmg_health");
		shot->damagegroup = HitgroupToDamagegroup(event->GetInt("hitgroup"));
		shot->health = event->GetInt("health");

		return true;
	}
	else if (event_name == "bullet_impact") {
		const int userid = EngineClient->GetPlayerForUserID(event->GetInt("userid"));

		if (userid != EngineClient->GetLocalPlayer())
			return false;

		RegisteredShot_t* shot = nullptr;

		for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
			if (it->acked)
				break;

			shot = &*it;
		}

		if (!shot || shot->impacts.size() > 4)
			return false; // manual fire

		Vector point(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

		shot->impacts.push_back(point);
		shot->ack_tick = GlobalVars->tickcount;

		float min_distance = (shot->hit_point - shot->target_pos).LengthSqr();
		float cur_distance = (point - shot->target_pos).LengthSqr();

		if (cur_distance < min_distance)
			shot->hit_point = point;

		return false;
	}
	else if (event_name == "player_death") {
		const int userid = EngineClient->GetPlayerForUserID(event->GetInt("userid"));
		const int attacker = EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

		if (userid == EngineClient->GetLocalPlayer()) {
			for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
				if (it->acked)
					break;

				if (it->ack_tick != 0)
					continue;

				it->acked = true;
				it->unregistered = true;
				it->death = true;
				it->miss_reason = "death";

				Console->ArcticTag();
				Console->ColorPrint("missed shot due to ", Color(230, 230, 230));
				Console->ColorPrint("death\n", Color(255, 20, 20));

				for (auto& callback : Lua->hooks.getHooks(LUA_AIM_ACK))
					callback.func(*it);
			}

			return false;
		}

		for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
			if (it->acked)
				break;

			if (it->record->player->EntIndex() == userid && attacker != EngineClient->GetLocalPlayer()) {
				it->ack_tick = GlobalVars->tickcount;
				it->player_death = true;
			}
		}
		return false;
	}
}

void CShotManager::OnNetUpdate() {
	for (auto it = m_RegisteredShots.rbegin(); it != m_RegisteredShots.rend(); it++) {
		if (it->acked)
			break;

		if (!it->ack_tick || it->impacts.size() == 0) { // dont recieved events yet or unregistered
			if (GlobalVars->tickcount - it->shot_tick > 64) {
				it->acked = true;
				it->unregistered = true;
				it->miss_reason = "unregistered shot";
			}
			continue;
		}

		RegisteredShot_t* shot = &*it;

		shot->acked = true;
		shot->end_pos = shot->impacts.back();

		Vector direction;
		
		const int total_impacts = shot->impacts.size();
		if (total_impacts > 1) { // we can correct get correct shoot pos & angle
			direction = shot->impacts[total_impacts - 1] - shot->impacts[total_impacts - 2]; // impacts should be sorted by distance

			shot->angle = Math::VectorAngles(direction);
			shot->shoot_pos = EngineTrace->ClosestPoint(shot->impacts[total_impacts - 1], shot->impacts[total_impacts - 2], shot->client_shoot_pos); // correct shoot pos by trace
		}
		else {
			shot->angle = Math::VectorAngles(shot->impacts.back() - shot->client_shoot_pos);
			shot->shoot_pos = shot->client_shoot_pos;

			direction = (shot->end_pos - shot->shoot_pos).Normalized();
		}


		if (shot->player_death) {
			Console->Log("missed shot due to player death");
			it->miss_reason = "player death";
		}
		else {
			CBasePlayer* player = shot->record->player;
			LagRecord* backup_record = &LagCompensation->records(player->EntIndex()).back(); // just updated player, so latest record is correct

			LagCompensation->BacktrackEntity(shot->record);

			QAngle backup_angle = player->m_angEyeAngles();

			player->m_angEyeAngles() = shot->player_angle;
			shot->record->BuildMatrix();
			player->m_angEyeAngles() = backup_angle;

			memcpy(player->GetCachedBoneData().Base(), shot->record->clamped_matrix, player->GetCachedBoneData().Count() * sizeof(matrix3x4_t));

			Console->ArcticTag();
			if (shot->damage > 0) {
				Resolver->OnHit(player, shot->record);
				Console->ColorPrint(std::format("hit {}'s {}({}) for {}({}) ({} remaining) [mismatch: ", player->GetName(), GetDamagegroupName(shot->damagegroup), GetDamagegroupName(shot->wanted_damagegroup), shot->damage, shot->wanted_damage, shot->health), Color(240, 240, 240));

				if (config.visuals.esp.hitmarker->get())
					ESP::AddHitmarker(shot->hit_point);

				if (config.visuals.esp.damage_marker->get())
					ESP::AddDamageMarker(shot->hit_point + Vector(0, 0, 10), shot->damage);

				if (shot->wanted_damagegroup != shot->damagegroup) {
					CGameTrace trace;
					Ray_t ray(shot->shoot_pos, shot->shoot_pos + direction * 8192);

					if (!EngineTrace->ClipRayToPlayer(ray, MASK_SHOT_HULL | CONTENTS_GRATE, player, &trace) || trace.hit_entity != player) {
						Console->ColorPrint("correction", Color(200, 255, 0));
						it->miss_reason = "correction";
					}
					else {
						if (HitboxToDamagegroup(trace.hitgroup) == shot->wanted_damagegroup) {
							Console->ColorPrint("correction", Color(200, 255, 0));
							it->miss_reason = "correction";
						}
						else {
							if ((shot->shoot_pos - shot->client_shoot_pos).LengthSqr() > 1.f) {
								Console->ColorPrint("pred. error", Color(255, 200, 0));
								it->miss_reason = "prediction error";
							}
							else {
								Console->ColorPrint("spread", Color(255, 200, 0));
								it->miss_reason = "spread";
							}
						}
					}
				}
				else {
					Console->ColorPrint("none", Color(255, 255, 255));
				}

				Console->ColorPrint("]\n", Color(240, 240, 240));
			}
			else {
				CGameTrace trace;
				Ray_t ray(shot->shoot_pos, shot->shoot_pos + direction * 8192);

				Console->ColorPrint("missed shot due to ", Color(240, 240, 240));

				if (shot->shoot_pos.DistTo(shot->end_pos) + 10.f < shot->client_shoot_pos.DistTo(shot->target_pos)) {
					it->miss_reason = "occlusion";
					Console->ColorPrint("occlusion\n", Color(255, 200, 0));
				}
				else if (!EngineTrace->ClipRayToPlayer(ray, MASK_SHOT_HULL | CONTENTS_GRATE, player, &trace) || trace.hit_entity != player) {
					if ((shot->shoot_pos - shot->client_shoot_pos).LengthSqr() > 1.f || shot->hitchance == 1.f) {
						it->miss_reason = "prediction error";
						Console->ColorPrint("pred. error", Color(255, 200, 0));
						Console->ColorPrint(std::format(" [diff: {:.4f}]\n", (shot->shoot_pos - shot->client_shoot_pos).Q_Length()), Color(240, 240, 240));
					}
					else {
						it->miss_reason = "spread";
						Console->ColorPrint("spread\n", Color(255, 200, 0));
					}
				}
				else {
					if ((shot->hit_point - shot->target_pos).LengthSqr() < 64.f && shot->impacts.size() > 1) {
						it->miss_reason = "damage rejection";
						Console->ColorPrint("damage rejection\n", Color(255, 20, 20)); // correct naming: sin shluhi s gmom
					}
					else {
						auto& records = LagCompensation->records(shot->record->player->EntIndex());
						bool break_lag_comp = false;
						for (auto it = records.rbegin(); it != records.rend(); it++) {
							if (it->m_flSimulationTime < shot->record->m_flSimulationTime)
								break;

							if (it->breaking_lag_comp) {
								break_lag_comp = true;
								break;
							}
						}

						if (break_lag_comp || shot->safe_point) {
							it->miss_reason = "lagcomp failure";
							Console->ColorPrint("lagcomp failure\n", Color(200, 255, 0));
						}
						else {
							it->miss_reason = "correction";
							Console->ColorPrint("correction\n", Color(200, 255, 0));
						}

						Resolver->OnMiss(player, shot->record);
					}
				}
		}

			LagCompensation->BacktrackEntity(backup_record);
		}

		for (auto& callback : Lua->hooks.getHooks(LUA_AIM_ACK))
			callback.func(shot);
	}
}

void CShotManager::AddShot(const Vector& shoot_pos, const Vector& target_pos, int damage, int damagegroup, int hitchance, bool safe, LagRecord* record) {
	RegisteredShot_t* shot = &m_RegisteredShots.emplace_back();

	shot->client_shoot_pos = shoot_pos;
	shot->target_pos = target_pos;
	shot->client_angle = Math::VectorAngles(target_pos - shoot_pos);
	shot->shot_tick = GlobalVars->tickcount;
	shot->hitchance = hitchance;
	shot->backtrack = GlobalVars->tickcount - record->update_tick;
	shot->record = record;
	shot->player_angle = record->player->m_angEyeAngles();
	shot->wanted_damage = damage;
	shot->wanted_damagegroup = damagegroup;
	shot->safe_point = safe;

	for (auto& callback : Lua->hooks.getHooks(LUA_AIM_SHOT))
		callback.func(shot);

	while (m_RegisteredShots.size() > 8)
		m_RegisteredShots.erase(m_RegisteredShots.begin());
}

void CShotManager::Reset() {
	m_RegisteredShots.clear();
}