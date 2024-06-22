#pragma once
#include <array>

#include "../../SDK/Globals.h"
#include "../../SDK/Interfaces.h"

struct LagRecord;

class CAnimationSystem {
	struct interpolate_data_t {
		Vector origin;
		Vector net_origin;
		matrix3x4_t original_matrix[128];
		bool valid = false;
	};

	interpolate_data_t interpolate_data[64];
	CCSGOPlayerAnimationState unupdated_animstate[64]; // keep unupdated unimstates here, not in records

	matrix3x4_t local_matrix[128];
	matrix3x4_t prediction_matrix[128];
	AnimationLayer local_layers[13];
	CCSGOPlayerAnimationState* prediction_animstate = new CCSGOPlayerAnimationState;

	Vector sent_abs_origin;
public:
	void	FrameStageNotify(EClientFrameStage stage);
	void	OnCreateMove();

	matrix3x4_t* GetLocalBoneMatrix() { return local_matrix; };
	void	CorrectLocalMatrix(matrix3x4_t* mat, int size);
	Vector	GetLocalSentAbsOrigin() { return sent_abs_origin; };
	void	UpdatePredictionAnimation();
	CCSGOPlayerAnimationState* GetPredictionAnimstate() { return prediction_animstate; };
	matrix3x4_t* GetPredictionMatrix() { return prediction_matrix; };

	void	BuildMatrix(CBasePlayer* player, matrix3x4_t* boneToWorld, int maxBones, int mask, AnimationLayer* animlayers);
	void	DisableInterpolationFlags(CBasePlayer* player);
	void	UpdateAnimations(CBasePlayer* player, LagRecord* record, std::deque<LagRecord>& records);
	CCSGOPlayerAnimationState* GetUnupdatedAnimstate(int id) { return &unupdated_animstate[id]; };

	Vector	GetInterpolated(CBasePlayer* player);
	void	RunInterpolation();
	void	InterpolateModel(CBasePlayer* player, matrix3x4_t* matrix);
	void	ResetInterpolation();
	void	InvalidateInterpolation(int i);
};

extern CAnimationSystem* AnimationSystem;
