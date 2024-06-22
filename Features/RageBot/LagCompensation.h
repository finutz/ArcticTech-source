#pragma once

#include "../../SDK/Misc/Vector.h"
#include "../../SDK/Misc/QAngle.h"
#include "../../SDK/Misc/Matrix.h"
#include "../../SDK/Misc/CBasePlayer.h"
#include "Resolver.h"

class CBasePlayer;

struct LagRecord {
	CBasePlayer* player = nullptr;

	matrix3x4_t bone_matrix[MAXSTUDIOBONES];
	matrix3x4_t aim_matrix[MAXSTUDIOBONES];
	matrix3x4_t clamped_matrix[MAXSTUDIOBONES];
	matrix3x4_t opposite_matrix[MAXSTUDIOBONES];

	AnimationLayer animlayers[13];

	Vector m_vecOrigin;
	Vector m_vecVelocity;
	Vector m_vecMins = Vector(0, 0, 0);
	Vector m_vecMaxs = Vector(0, 0, 0);
	QAngle m_vecAbsAngles;
	Vector m_vecAbsOrigin;

	QAngle m_angEyeAngles;

	float m_flSimulationTime = 0.f;
	float m_flDuckAmout = 0.f;
	float m_flDuckSpeed = 0.f;
	float m_flCycle = 0.f;

	int m_nSequence = 0;
	int m_fFlags = 0;
	int m_nChokedTicks = 0;
	int update_tick = 0;

	bool shifting_tickbase = false;
	bool breaking_lag_comp = false;
	bool exploiting = false;
	bool invalid = false;
	bool shooting = false;

	bool bone_matrix_filled = false;

	ResolverData_t resolver_data;

	LagRecord* prev_record;

	void BuildMatrix();
};

class CLagCompensation {
	std::array<std::deque<LagRecord>, 64> lag_records;
	float max_simulation_time[64];
	int last_update_tick[64];

public:

	__forceinline std::deque<LagRecord>& records(int index) { return lag_records[index]; };

	LagRecord* BackupData(CBasePlayer* player);

	void RecordDataIntoTrack(CBasePlayer* player, LagRecord* record);
	void BacktrackEntity(LagRecord* record, bool copy_matrix = true, bool use_aim_matrix = false);
	void OnNetUpdate();
	void Reset(int index = -1);

	// Record helpers
	float GetLerpTime();
	bool ValidRecord(LagRecord* record);
	void Invalidate(int index);
};

extern CLagCompensation* LagCompensation;