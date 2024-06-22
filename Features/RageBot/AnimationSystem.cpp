#include "AnimationSystem.h"
#include "LagCompensation.h"
#include "../AntiAim/AntiAim.h"
#include "Resolver.h"
#include "Exploits.h"
#include "../Lua/Bridge/Bridge.h"
#include "../Misc/Prediction.h"

void CAnimationSystem::CorrectLocalMatrix(matrix3x4_t* mat, int size) {
	Utils::MatrixMove(mat, size, sent_abs_origin, Cheat.LocalPlayer->GetAbsOrigin());
}

void FixLegMovement(AnimationLayer* server_layers) {
	if (config.antiaim.misc.leg_movement->get() != 1)
		return;

	if (!(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	if (Cheat.LocalPlayer->m_vecVelocity().LengthSqr() < 4096)
		return;

	CCSGOPlayerAnimationState* animstate = Cheat.LocalPlayer->GetAnimstate();

	float delta = Math::AngleDiff(ctx.cmd->viewangles.yaw, ctx.leg_slide_angle.yaw);
	float moveYaw = Math::AngleDiff(Math::VectorAngles(Cheat.LocalPlayer->m_vecVelocity()).yaw, ctx.cmd->viewangles.yaw);

	Cheat.LocalPlayer->m_flPoseParameter()[STRAFE_YAW] = Math::AngleToPositive(Math::AngleNormalize(moveYaw + delta)) / 360.f;

	AnimationLayer* layers = Cheat.LocalPlayer->GetAnimlayers();

	if (server_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight > layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight)
		layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight = server_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight;
}

void CAnimationSystem::OnCreateMove() {
	CCSGOPlayerAnimationState* animstate = Cheat.LocalPlayer->GetAnimstate();

	AnimationLayer animlayers_backup[13];
	memcpy(animlayers_backup, Cheat.LocalPlayer->GetAnimlayers(), sizeof(AnimationLayer) * 13);

	for (auto& cb : Lua->hooks.getHooks(LUA_PRE_ANIMUPDATE))
		cb.func(Cheat.LocalPlayer);

	QAngle vangle = ctx.cmd->viewangles;
	if (ctx.send_packet && ctx.force_shot_angle) {
		vangle = ctx.shot_angles;
		ctx.force_shot_angle = false;
	}	

	if (animstate->nLastUpdateFrame >= GlobalVars->framecount)
		animstate->nLastUpdateFrame = GlobalVars->framecount - 1;

	if (animstate->flLastUpdateTime >= GlobalVars->curtime)
		animstate->flLastUpdateTime = GlobalVars->curtime - GlobalVars->interval_per_tick;

	Cheat.LocalPlayer->UpdateAnimationState(animstate, vangle);
	animstate->bLanding = Cheat.LocalPlayer->GetAnimlayers()[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].m_flWeight > 0.f && animstate->bOnGround; // fuck valve broken code that sets bLanding to false
	memcpy(local_layers, Cheat.LocalPlayer->GetAnimlayers(), sizeof(AnimationLayer) * 13);

	if (ctx.send_packet && !Exploits->IsHidingShot() && animstate->nLastUpdateFrame == GlobalVars->framecount) {
		FixLegMovement(animlayers_backup);

		Cheat.LocalPlayer->SetAbsAngles(QAngle(0, animstate->flFootYaw, 0));
		sent_abs_origin = Cheat.LocalPlayer->GetAbsOrigin();

		for (auto& cb : Lua->hooks.getHooks(LUA_POST_ANIMUPDATE))
			cb.func(Cheat.LocalPlayer);

		ctx.setup_bones_angle = vangle;

		BuildMatrix(Cheat.LocalPlayer, local_matrix, 128, BONE_USED_BY_ANYTHING, nullptr);
	}

	memcpy(Cheat.LocalPlayer->GetAnimlayers(), animlayers_backup, sizeof(AnimationLayer) * 13);
}

void CAnimationSystem::UpdatePredictionAnimation() {
	CCSGOPlayerAnimationState* animstate = Cheat.LocalPlayer->GetAnimstate();

	std::array<float, 24> poseparam_backup = Cheat.LocalPlayer->m_flPoseParameter();
	AnimationLayer animlayer_backup[13];

	memcpy(animlayer_backup, Cheat.LocalPlayer->GetAnimlayers(), sizeof(AnimationLayer) * 13);

	Cheat.LocalPlayer->m_flPoseParameter()[12] = (ctx.cmd->viewangles.pitch + 90.f) / 180.f;
	Cheat.LocalPlayer->SetAbsAngles(QAngle(0, animstate->flFootYaw));

	BuildMatrix(Cheat.LocalPlayer, prediction_matrix, 128, BONE_USED_BY_HITBOX, local_layers);

	Cheat.LocalPlayer->m_flPoseParameter() = poseparam_backup;
	memcpy(Cheat.LocalPlayer->GetAnimlayers(), animlayer_backup, sizeof(AnimationLayer) * 13);
}

void CAnimationSystem::FrameStageNotify(EClientFrameStage stage) {
	switch (stage)
	{
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		for (int i = 0; i < ClientState->m_nMaxClients; ++i) {
			CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(i));

			if (player && player != Cheat.LocalPlayer)
				DisableInterpolationFlags(player);
		}
		break;
	case FRAME_NET_UPDATE_END:
		break;
	case FRAME_RENDER_START:
		break;
	default:
		break;
	}
}

void CAnimationSystem::BuildMatrix(CBasePlayer* player, matrix3x4_t* boneToWorld, int maxBones, int mask, AnimationLayer* animlayers) {

	player->InvalidateBoneCache();

	if (animlayers != nullptr)
		memcpy(player->GetAnimlayers(), animlayers, sizeof(AnimationLayer) * 13);

	bool backupMaintainSequenceTransitions = player->m_bMaintainSequenceTransitions();
	int backupEffects = player->m_fEffects();

	player->m_fEffects() |= EF_NOINTERP; // Disable interp
	player->m_bMaintainSequenceTransitions() = false; // uhhhh, idk

	hook_info.setup_bones = true;
	player->SetupBones(boneToWorld, maxBones, mask, Cheat.LocalPlayer == player ? GlobalVars->curtime : player->m_flSimulationTime());
	hook_info.setup_bones = false;

	player->m_fEffects() = backupEffects;
	player->m_bMaintainSequenceTransitions() = backupMaintainSequenceTransitions;
}

void CAnimationSystem::DisableInterpolationFlags(CBasePlayer* player) {
	auto& var_mapping = player->m_VarMapping();

	for (int i = 0; i < var_mapping.m_nInterpolatedEntries; ++i)
		var_mapping.m_Entries[i].m_bNeedsToInterpolate = false;
}

void CAnimationSystem::UpdateAnimations(CBasePlayer* player, LagRecord* record, std::deque<LagRecord>& records) {
	CCSGOPlayerAnimationState* animstate = player->GetAnimstate();
	const int idx = player->EntIndex();

	if (!animstate)
		return;
	
	record->player = player;
	record->m_vecAbsOrigin = player->m_vecOrigin();

	unupdated_animstate[idx] = *animstate;

	auto backupRealtime = GlobalVars->realtime;
	auto backupCurtime = GlobalVars->curtime;
	auto backupFrametime = GlobalVars->frametime;
	auto backupAbsFrametime = GlobalVars->absoluteframetime;
	auto backupInterp = GlobalVars->interpolation_amount;
	auto backupTickcount = GlobalVars->tickcount;
	auto backupFramecount = GlobalVars->framecount;
	auto backupAbsOrigin = player->GetAbsOrigin();
	auto backupAbsVelocity = player->m_vecAbsVelocity();
	auto backupLBY = player->m_flLowerBodyYawTarget();
	auto nOcclusionMask = player->m_nOcclusionFlags();
	auto nOcclusionFrame = player->m_nOcclusionFrame();
	auto backupAbsAngles = player->GetAbsAngles();

	GlobalVars->realtime = player->m_flSimulationTime();
	GlobalVars->curtime = player->m_flSimulationTime();
	GlobalVars->frametime = GlobalVars->interval_per_tick;
	GlobalVars->absoluteframetime = GlobalVars->interval_per_tick;
	GlobalVars->interpolation_amount = 0.f;
	GlobalVars->tickcount = TIME_TO_TICKS(player->m_flSimulationTime());
	GlobalVars->framecount = TIME_TO_TICKS(player->m_flSimulationTime());

	memcpy(record->animlayers, player->GetAnimlayers(), 13 * sizeof(AnimationLayer));
	record->animlayers[12].m_flWeight = 0.f;
	auto pose_params = player->m_flPoseParameter();

	player->m_iEFlags() &= ~(EFL_DIRTY_ABSTRANSFORM | EFL_DIRTY_ABSVELOCITY);

	if (!player->IsTeammate()) {
		player->m_BoneAccessor().m_ReadableBones = 0;
		player->m_BoneAccessor().m_WritableBones = 0;

		player->m_nOcclusionFrame() = 0;
		player->m_nOcclusionFlags() = 0;
	}

	player->SetAbsVelocity(player->m_vecVelocity());
	player->SetAbsOrigin(player->m_vecOrigin());

	if (!player->IsTeammate()) {
		Resolver->Run(player, record, records);

		*animstate = unupdated_animstate[idx];

		Resolver->Apply(record);
		player->UpdateClientSideAnimation();
	}
	else {
		player->UpdateClientSideAnimation();
	}

	if (!(player->m_fFlags() & FL_ONGROUND)) {
		animstate->flDurationInAir = (cvars.sv_jump_impulse->GetFloat() - player->m_flFallVelocity()) / cvars.sv_gravity->GetFloat();
	}

	hook_info.disable_clamp_bones = true;
	BuildMatrix(player, record->aim_matrix, 128, BONE_USED_BY_ANYTHING, record->animlayers);
	hook_info.disable_clamp_bones = false;

	interpolate_data_t* lerp_data = &interpolate_data[idx];
	lerp_data->net_origin = player->m_vecOrigin();

	memcpy(record->bone_matrix, record->aim_matrix, sizeof(matrix3x4_t) * 128);
	player->ClampBonesInBBox(record->bone_matrix, BONE_USED_BY_ANYTHING);
	memcpy(lerp_data->original_matrix, record->bone_matrix, sizeof(matrix3x4_t) * 128);

	record->bone_matrix_filled = true;

	if (player->IsEnemy()) {
		float deltaOriginal = Math::AngleDiff(animstate->flEyeYaw, animstate->flFootYaw);
		float eyeYawNew = Math::AngleNormalize(animstate->flEyeYaw + deltaOriginal);
		player->SetAbsAngles(QAngle(0, eyeYawNew, 0));
		player->m_flPoseParameter()[BODY_YAW] = 1.f - player->m_flPoseParameter()[BODY_YAW]; // opposite side

		BuildMatrix(player, record->opposite_matrix, 128, BONE_USED_BY_HITBOX, record->animlayers);

		if (config.ragebot.aimbot.show_aimpoints->get())
			DebugOverlay->AddBoxOverlay(player->GetHitboxCenter(HITBOX_HEAD, record->opposite_matrix), Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(), 12, 255, 12, 160, 0.1f);
	}

	player->m_nOcclusionFrame() = nOcclusionFrame;
	player->m_nOcclusionFlags() = nOcclusionMask;

	player->SetAbsOrigin(backupAbsOrigin);
	player->m_vecAbsVelocity() = backupAbsVelocity;

	GlobalVars->realtime = backupRealtime;
	GlobalVars->curtime = backupCurtime;
	GlobalVars->frametime = backupFrametime;
	GlobalVars->absoluteframetime = backupAbsFrametime;
	GlobalVars->interpolation_amount = backupInterp;
	GlobalVars->tickcount = backupTickcount;
	GlobalVars->framecount = backupFramecount;
	
	player->SetAbsAngles(backupAbsAngles);
	player->m_flLowerBodyYawTarget() = backupLBY;
	memcpy(player->GetAnimlayers(), record->animlayers, sizeof(AnimationLayer) * 13);
	memcpy(player->GetCachedBoneData().Base(), record->bone_matrix, sizeof(matrix3x4_t) * player->GetCachedBoneData().Count());
	player->m_flPoseParameter() = pose_params;
}

Vector CAnimationSystem::GetInterpolated(CBasePlayer* player) {
	return interpolate_data[player->EntIndex()].origin;
}

void CAnimationSystem::RunInterpolation() {
	for (int i = 0; i < 64; i++) {
		CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(i));

		if (!player || player == Cheat.LocalPlayer || !player->IsAlive() || player->m_bDormant())
			continue;

		interpolate_data_t* data = &interpolate_data[i];

		if ((data->net_origin - data->origin).LengthSqr() > 8192) {
			data->origin = data->net_origin;
			data->valid = false;
			continue;
		}

		float lerp_amt = 24.f;

		if (Cheat.LocalPlayer && Cheat.LocalPlayer->IsAlive() && player->IsEnemy())
			lerp_amt = 36.f; // speed up interpolation

		data->valid = true;
		data->origin += (data->net_origin - data->origin) * std::clamp(GlobalVars->frametime * lerp_amt, 0.f, 0.8f);
	}
}

void CAnimationSystem::InterpolateModel(CBasePlayer* player, matrix3x4_t* matrix) {
	if (player == Cheat.LocalPlayer)
		return;

	interpolate_data_t* data = &interpolate_data[player->EntIndex()];

	if (!data->valid)
		return;

	Utils::MatrixMove(data->original_matrix, matrix, player->GetCachedBoneData().Count(), data->net_origin, data->origin);
}

void CAnimationSystem::ResetInterpolation() {
	for (int i = 0; i < 64; i++)
		interpolate_data[i].valid = false;
}

void CAnimationSystem::InvalidateInterpolation(int i) {
	interpolate_data[i].valid = false;
}

CAnimationSystem* AnimationSystem = new CAnimationSystem;