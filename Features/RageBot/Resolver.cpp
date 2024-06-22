#include "Resolver.h"

#include <algorithm>
#include "../../SDK/Interfaces.h"
#include "../../SDK/Misc/CBasePlayer.h"
#include "LagCompensation.h"
#include "../../SDK/Globals.h"
#include "AnimationSystem.h"


CResolver* Resolver = new CResolver;

float FindAvgYaw(const std::deque<LagRecord>& records) {
	float sin_sum = 0.f;
	float cos_sum = 0.f;

	for (int i = records.size() - 2; i > records.size() - 10; i--) {
		const LagRecord* record = &records.at(i);
		float eyeYaw = record->m_angEyeAngles.yaw;

		sin_sum += std::sinf(DEG2RAD(eyeYaw));
		cos_sum += std::cosf(DEG2RAD(eyeYaw));
	}

	return RAD2DEG(std::atan2f(sin_sum, cos_sum));
}

void CResolver::Reset(CBasePlayer* pl) {
	if (pl) {
		brute_force_data[pl->EntIndex()].reset();
		return;
	}

	for (int i = 0; i < 64; ++i) {
		brute_force_data[i].reset();
	}
}

R_PlayerState CResolver::DetectPlayerState(CBasePlayer* player, AnimationLayer* animlayers) {
	if (!(player->m_fFlags() & FL_ONGROUND))
		return R_PlayerState::AIR;

	CCSGOPlayerAnimationState* animstate = player->GetAnimstate();

	if (player->m_vecVelocity().Length2DSqr() > 256.f && animstate->flWalkToRunTransition > 0.8f && animlayers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate > 0.0001f)
		return R_PlayerState::MOVING;

	return R_PlayerState::STANDING;
}

R_AntiAimType CResolver::DetectAntiAim(CBasePlayer* player, const std::deque<LagRecord>& records) {
	if (records.size() < 12)
		return R_AntiAimType::NONE;

	int jitteredRecords = 0;
	int staticRecords = 0;
	float avgDelta = 0.f;
	float prevEyeYaw = player->m_angEyeAngles().yaw;

	for (int i = records.size() - 2; i > records.size() - 10; i--) {
		const LagRecord* record = &records.at(i);
		float eyeYaw = record->m_angEyeAngles.yaw;

		float delta = std::abs(Math::AngleDiff(eyeYaw, prevEyeYaw));

		avgDelta += delta;

		if (delta > 18.f)
			jitteredRecords++;
		else
			staticRecords++;

		prevEyeYaw = eyeYaw;
	}

	if (jitteredRecords > staticRecords)
		return R_AntiAimType::JITTER;

	if (avgDelta * 0.5f < 30.f)
		return R_AntiAimType::STATIC;

	return R_AntiAimType::UNKNOWN;
}

void CResolver::SetupResolverLayers(CBasePlayer* player, LagRecord* record) {
	CCSGOPlayerAnimationState* animstate = player->GetAnimstate();

	float eyeYaw = player->m_angEyeAngles().yaw;

	float zeroYaw = Math::AngleNormalize(eyeYaw);
	float posYaw = Math::AngleNormalize(eyeYaw + player->GetMaxDesyncDelta());
	float negYaw = Math::AngleNormalize(eyeYaw - player->GetMaxDesyncDelta());

	// zero delta
	std::memcpy(animstate, AnimationSystem->GetUnupdatedAnimstate(player->EntIndex()), sizeof(CCSGOPlayerAnimationState));
	animstate->flFootYaw = zeroYaw;

	player->UpdateAnimationState(animstate, record->m_angEyeAngles, true);
	memcpy(record->resolver_data.animlayers[0], player->GetAnimlayers(), sizeof(AnimationLayer) * 13);

	// positive delta
	std::memcpy(animstate, AnimationSystem->GetUnupdatedAnimstate(player->EntIndex()), sizeof(CCSGOPlayerAnimationState));
	animstate->flFootYaw = posYaw;

	player->UpdateAnimationState(animstate, record->m_angEyeAngles, true);
	memcpy(record->resolver_data.animlayers[1], player->GetAnimlayers(), sizeof(AnimationLayer) * 13);

	// negative delta
	std::memcpy(animstate, AnimationSystem->GetUnupdatedAnimstate(player->EntIndex()), sizeof(CCSGOPlayerAnimationState));
	animstate->flFootYaw = negYaw;

	player->UpdateAnimationState(animstate, record->m_angEyeAngles, true);
	memcpy(record->resolver_data.animlayers[2], player->GetAnimlayers(), sizeof(AnimationLayer) * 13);
}

void CResolver::DetectFreestand(CBasePlayer* player, LagRecord* record, const std::deque<LagRecord>& records) {
	if (records.size() < 16)
		return;

	Vector eyePos = player->m_vecOrigin() + Vector(0, 0, 64 - player->m_flDuckAmount() * 16.f);

	Vector forward = (Cheat.LocalPlayer->m_vecOrigin() - player->m_vecOrigin()).Q_Normalized();

	float notModifiedYaw = FindAvgYaw(records);

	Vector right = Math::AngleVectors(QAngle(0, notModifiedYaw + 90.f, 0));

	Vector negPos = eyePos - right * 20.f;
	Vector posPos = eyePos + right * 20.f;

	Vector local_eye_pos = Cheat.LocalPlayer->GetEyePosition();
	local_eye_pos.z -= 0.5f;

	CTraceFilterWorldAndPropsOnly filter;
	Ray_t rayNeg(negPos, local_eye_pos);
	Ray_t rayPos(posPos, local_eye_pos);
	CGameTrace negTrace, posTrace;

	EngineTrace->TraceRay(rayNeg, MASK_SHOT_HULL | CONTENTS_GRATE, &filter, &negTrace);
	EngineTrace->TraceRay(rayPos, MASK_SHOT_HULL | CONTENTS_GRATE, &filter, &posTrace);

	if (negTrace.startsolid && posTrace.startsolid) {
		record->resolver_data.side = 0;
		record->resolver_data.resolver_type = ResolverType::NONE;
		return;
	}
	else if (negTrace.startsolid) {
		record->resolver_data.side = -1;
		record->resolver_data.resolver_type = ResolverType::FREESTAND;
		return;
	}
	else if (posTrace.startsolid) {
		record->resolver_data.side = 1;
		record->resolver_data.resolver_type = ResolverType::FREESTAND;
		return;
	}

	if (negTrace.fraction == 1.f && posTrace.fraction == 1.f) {
		record->resolver_data.side = 0;
		record->resolver_data.resolver_type = ResolverType::NONE;
		return;
	}
	record->resolver_data.side = negTrace.fraction < posTrace.fraction ? -1 : 1;
	record->resolver_data.resolver_type = ResolverType::FREESTAND;
}

void CResolver::Apply(LagRecord* record) {
	if (record->resolver_data.side != 0) {
		float body_yaw = record->player->GetMaxDesyncDelta() * record->resolver_data.side;
		auto state = record->player->GetAnimstate();

		state->flFootYaw = Math::AngleNormalize(state->flEyeYaw + body_yaw);
	}
}

void CResolver::Run(CBasePlayer* player, LagRecord* record, std::deque<LagRecord>& records) {
	if (GameRules()->IsFreezePeriod() || player->m_fFlags() & FL_FROZEN || !Cheat.LocalPlayer->IsAlive() || record->shooting)
		return;

	LagRecord* prevRecord = record->prev_record;

	if (!record->m_nChokedTicks || player->m_bIsDefusing()) {
		record->resolver_data.side = 0;
		record->resolver_data.resolver_type = ResolverType::NONE;
		return;
	}

	record->resolver_data.player_state = DetectPlayerState(player, record->animlayers);
	record->resolver_data.antiaim_type = DetectAntiAim(player, records);

	SetupResolverLayers(player, record);

	record->resolver_data.resolver_type = ResolverType::NONE;

	record->resolver_data.delta_center = abs(record->animlayers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - record->resolver_data.animlayers[0][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate) * 1500.f;
	record->resolver_data.delta_positive = abs(record->animlayers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - record->resolver_data.animlayers[1][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate) * 1000.f;
	record->resolver_data.delta_negative = abs(record->animlayers[ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate - record->resolver_data.animlayers[2][ANIMATION_LAYER_MOVEMENT_MOVE].m_flPlaybackRate) * 1000.f;

	float flLastDelta = 1000.f;

	if (record->resolver_data.delta_center < flLastDelta) {
		record->resolver_data.resolver_type = ResolverType::NONE;
		record->resolver_data.side = 0;
		flLastDelta = record->resolver_data.delta_center;
	}

	if (record->resolver_data.delta_positive < flLastDelta) {
		record->resolver_data.resolver_type = ResolverType::ANIM;
		record->resolver_data.side = 1;
		flLastDelta = record->resolver_data.delta_positive;
	}

	if (record->resolver_data.delta_negative < flLastDelta) {
		record->resolver_data.resolver_type = ResolverType::ANIM;
		record->resolver_data.side = -1;
		flLastDelta = record->resolver_data.delta_negative;
	}


	float vel_sqr = player->m_vecVelocity().LengthSqr();

	if (vel_sqr < 64.f || 
		player->GetAnimstate()->flWalkToRunTransition < 0.8f || 
		record->resolver_data.resolver_type == ResolverType::NONE || 
		!(player->m_fFlags() & FL_ONGROUND)
		) 
	{
		if (record->resolver_data.antiaim_type == R_AntiAimType::JITTER && records.size() > 16) {
			float eyeYaw = player->m_angEyeAngles().yaw;
			float prevEyeYaw = FindAvgYaw(records);
			float delta = Math::AngleDiff(eyeYaw, prevEyeYaw);

			if (delta < 0.f)
				record->resolver_data.side = -1;
			else
				record->resolver_data.side = 1;

			record->resolver_data.resolver_type = ResolverType::LOGIC;
		}
		else {
			DetectFreestand(player, record, records);
		}
	}

	BruteForceData_t* bf_data = &brute_force_data[player->EntIndex()];
	if (bf_data->use && GlobalVars->realtime - bf_data->last_shot < 5.f && record->resolver_data.resolver_type != ResolverType::ANIM && record->resolver_data.resolver_type != ResolverType::LOGIC && bf_data->current_side != 0) {
		record->resolver_data.side = bf_data->current_side;
		record->resolver_data.resolver_type = ResolverType::BRUTEFORCE;
	}

	if (vel_sqr > 0.64f && vel_sqr < 4.f && record->resolver_data.antiaim_type == R_AntiAimType::STATIC && record->resolver_data.side == 0)
		record->resolver_data.side = -1;

	Apply(record);
}

void CResolver::OnMiss(CBasePlayer* player, LagRecord* record) {
	BruteForceData_t* bf_data = &brute_force_data[player->EntIndex()];

	bf_data->current_side = (record->resolver_data.side == 0) ? 1 : -record->resolver_data.side;
	bf_data->use = true;
	bf_data->last_shot = GlobalVars->realtime;
}

void CResolver::OnHit(CBasePlayer* player, LagRecord* record) {
	BruteForceData_t* bf_data = &brute_force_data[player->EntIndex()];

	bf_data->use = true;
	bf_data->current_side = record->resolver_data.side;
	bf_data->last_shot = GlobalVars->realtime;
}