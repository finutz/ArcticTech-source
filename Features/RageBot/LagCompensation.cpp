#include "LagCompensation.h"
#include "AnimationSystem.h"
#include "../Misc/Prediction.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../AntiAim/AntiAim.h"
#include "../../Utils/Utils.h"
#include <algorithm>
#include "../../SDK/NetMessages.h"
#include "../Visuals/ESP.h"
#include "Exploits.h"

LagRecord* CLagCompensation::BackupData(CBasePlayer* player) {
	LagRecord* record = new LagRecord;

	record->player = player;
	record->m_vecAbsOrigin = player->GetAbsOrigin();
	RecordDataIntoTrack(player, record);

	return record;
}

void CLagCompensation::RecordDataIntoTrack(CBasePlayer* player, LagRecord* record) {
	record->player = player;

	record->m_angEyeAngles = player->m_angEyeAngles();
	record->m_flSimulationTime = player->m_flSimulationTime();
	record->m_vecOrigin = player->m_vecOrigin();
	record->m_fFlags = player->m_fFlags();
	record->m_flCycle = player->m_flCycle();
	record->m_nSequence = player->m_nSequence();
	record->m_flDuckAmout = player->m_flDuckAmount();
	record->m_flDuckSpeed = player->m_flDuckSpeed();
	record->m_vecMaxs = player->m_vecMaxs();
	record->m_vecMins = player->m_vecMins();
	record->m_vecVelocity = player->m_vecVelocity();
	record->m_vecAbsAngles = player->GetAbsAngles();

	if (!record->bone_matrix_filled) {
		memcpy(record->bone_matrix, player->GetCachedBoneData().Base(), sizeof(matrix3x4_t) * player->GetCachedBoneData().Count());
		record->bone_matrix_filled = true;
	}

	if (player->GetActiveWeapon())
		if (player->GetActiveWeapon()->m_fLastShotTime() <= player->m_flSimulationTime())
			if (player->GetActiveWeapon()->m_fLastShotTime() > player->m_flOldSimulationTime())
				record->shooting = true;

	memcpy(record->animlayers, player->GetAnimlayers(), sizeof(AnimationLayer) * 13);
}

void CLagCompensation::BacktrackEntity(LagRecord* record, bool copy_matrix, bool use_aim_matrix) {
	CBasePlayer* player = record->player;

	//player->m_flSimulationTime() = record->m_flSimulationTime;
	player->m_vecOrigin() = record->m_vecOrigin;
	player->SetAbsOrigin(record->m_vecAbsOrigin);
	player->m_fFlags() = record->m_fFlags;
	player->m_flCycle() = record->m_flCycle;
	player->m_nSequence() = record->m_nSequence;
	player->m_flDuckAmount() = record->m_flDuckAmout;
	player->m_flDuckSpeed() = record->m_flDuckSpeed;
	player->m_vecVelocity() = record->m_vecVelocity;
	player->SetAbsAngles(record->m_vecAbsAngles);
	player->ForceBoneCache();

	player->SetCollisionBounds(record->m_vecMins, record->m_vecMaxs);

	if (copy_matrix) {
		if (use_aim_matrix) {
			memcpy(player->GetCachedBoneData().Base(), record->clamped_matrix, player->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
		}
		else {
			memcpy(player->GetCachedBoneData().Base(), record->bone_matrix, player->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
		}
	}
}

void LagRecord::BuildMatrix() {
	memcpy(clamped_matrix, aim_matrix, 128 * sizeof(matrix3x4_t));

	if (config.antiaim.angles.legacy_desync->get())
		return;

	auto backup_eye_angle = player->m_angEyeAngles();

	INetChannelInfo* nci = EngineClient->GetNetChannelInfo();

	if (prev_record && 
		resolver_data.antiaim_type == R_AntiAimType::JITTER && 
		std::abs(Math::AngleDiff(backup_eye_angle.yaw, prev_record->m_angEyeAngles.yaw)) > 32.f &&
		std::abs(Math::AngleDiff(backup_eye_angle.yaw, m_angEyeAngles.yaw)) < 32.f) // retarded jitter fix
		player->m_angEyeAngles() = prev_record->m_angEyeAngles;

	if (config.ragebot.aimbot.roll_resolver->get()) {
		player->m_angEyeAngles().roll = config.ragebot.aimbot.roll_angle->get() * (resolver_data.side != 0 ? resolver_data.side : 1);
	}

	player->ClampBonesInBBox(clamped_matrix, BONE_USED_BY_ANYTHING);

	player->m_angEyeAngles() = backup_eye_angle;
}

void CLagCompensation::OnNetUpdate() {
	if (!Cheat.InGame)
		return;

	INetChannel* nc = ClientState->m_NetChannel;
	auto nci = EngineClient->GetNetChannelInfo();

	for (int i = 0; i < ClientState->m_nMaxClients; i++) {
		CBasePlayer* pl = (CBasePlayer*)EntityList->GetClientEntity(i);

		if (!pl || !pl->IsAlive() || pl == Cheat.LocalPlayer || pl->m_bDormant())
			continue;

		auto& records = lag_records[pl->EntIndex()];

		if (records.empty() || pl->m_flSimulationTime() != pl->m_flOldSimulationTime()) {
			LagRecord* prev_record = !records.empty() ? &records.back() : nullptr;
			LagRecord* new_record = &records.emplace_back();

			new_record->prev_record = prev_record;
			new_record->update_tick = GlobalVars->tickcount;
			new_record->m_nChokedTicks = new_record->update_tick - (prev_record ? prev_record->update_tick : new_record->update_tick) - 1;
			new_record->m_flSimulationTime = pl->m_flSimulationTime();

			new_record->shifting_tickbase = max_simulation_time[i] >= new_record->m_flSimulationTime;
			new_record->exploiting = GlobalVars->curtime - pl->m_flSimulationTime() > TICKS_TO_TIME(10.f) || (prev_record && prev_record->shifting_tickbase);

			if (new_record->m_flSimulationTime > max_simulation_time[i] || abs(max_simulation_time[i] - new_record->m_flSimulationTime) > 3.f)
				max_simulation_time[i] = new_record->m_flSimulationTime;

			last_update_tick[i] = GlobalVars->tickcount;

			AnimationSystem->UpdateAnimations(pl, new_record, records);
			RecordDataIntoTrack(pl, new_record);

			if (prev_record)
				new_record->breaking_lag_comp = (prev_record->m_vecOrigin - new_record->m_vecOrigin).LengthSqr() > 4096.f;

			if (config.visuals.esp.shared_esp->get() && !EngineClient->IsVoiceRecording() && nc) {
				if (config.visuals.esp.share_with_enemies->get() || !pl->IsTeammate()) {
					SharedESP_t msg;

					player_info_t pinfo;
					EngineClient->GetPlayerInfo(i, &pinfo);

					msg.m_iPlayer = pinfo.userId;
					msg.m_ActiveWeapon = pl->GetActiveWeapon() ? pl->GetActiveWeapon()->m_iItemDefinitionIndex() : 0;
					msg.m_iHealth = pl->m_iHealth();
					msg.m_vecOrigin = new_record->m_vecOrigin;

					NetMessages->SendNetMessage((SharedVoiceData_t*)&msg);
				}
			}

			while (records.size() > (pl->IsTeammate() ? 12 : (TIME_TO_TICKS(0.4f) + 13))) { // super puper proper lagcomp
				records.pop_front();
			}
		}

		INetChannelInfo* nci = EngineClient->GetNetChannelInfo();
		if (config.visuals.esp.show_server_hitboxes->get() && nci && nci->IsLoopback())
			pl->DrawServerHitboxes(GlobalVars->interval_per_tick, true);
	}
}

float CLagCompensation::GetLerpTime() {
	static const auto cl_interp = CVar->FindVar("cl_interp");
	static const auto cl_updaterate = CVar->FindVar("cl_updaterate");
	static const auto sv_minupdaterate = CVar->FindVar("sv_minupdaterate");
	static const auto sv_maxupdaterate = CVar->FindVar("sv_maxupdaterate");
	static const auto cl_interp_ratio = CVar->FindVar("cl_interp_ratio");
	static const auto sv_min_interp_ratio = CVar->FindVar("sv_client_min_interp_ratio");
	static const auto sv_max_interp_ratio = CVar->FindVar("sv_client_max_interp_ratio");

	const float update_rate = std::clamp<float>(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
	const float interp_ratio = std::clamp<float>(cl_interp_ratio->GetFloat(), sv_min_interp_ratio->GetFloat(), sv_max_interp_ratio->GetFloat());

	return std::clamp<float>(interp_ratio / update_rate, cl_interp->GetFloat(), 1.f);
}

bool CLagCompensation::ValidRecord(LagRecord* record) {
	if (!record || !record->player || record->shifting_tickbase || record->breaking_lag_comp || record->invalid)
		return false;

	auto nci = EngineClient->GetNetChannelInfo();

	if (!nci)
		return false;

	const float latency = nci->GetLatency(FLOW_INCOMING) + nci->GetLatency(FLOW_OUTGOING);

	int server_tickcount = ctx.corrected_tickbase + TIME_TO_TICKS(latency);

	if (ctx.fake_duck)
		server_tickcount += 14 - ClientState->m_nChokedCommands;

	const float lerp_time = GetLerpTime();
	const float delta_time = std::clamp(latency + lerp_time, 0.f, cvars.sv_maxunlag->GetFloat()) - (TICKS_TO_TIME(ctx.corrected_tickbase) - record->m_flSimulationTime);

	if (fabs(delta_time) > 0.2f)
		return false;

	/// omg v0lvo broke this check but i want to add it because i want to be like Soufiw
	const float dead_time = TICKS_TO_TIME(server_tickcount) - 0.2f;
	if (record->m_flSimulationTime + lerp_time < dead_time)
		return false;

	return true;
}

void CLagCompensation::Reset(int index) {
	if (index != -1) {
		lag_records[index].clear();
		max_simulation_time[index] = 0.f;
		last_update_tick[index] = 0;
	}
	else {
		for (int i = 0; i < lag_records.size(); i++) {
			lag_records[i].clear();
			max_simulation_time[i] = 0.f;
			last_update_tick[i] = 0;
		}
	}
}

void CLagCompensation::Invalidate(int index) {
	for (auto& record : lag_records[index])
		record.invalid = true;
}

CLagCompensation* LagCompensation = new CLagCompensation;