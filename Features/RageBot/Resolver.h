#pragma once

#include <deque>

#include "../../SDK/Misc/CBasePlayer.h"

struct LagRecord;

enum class R_PlayerState {
	STANDING,
	MOVING,
	AIR
};

enum class R_AntiAimType {
	NONE,
	STATIC,
	JITTER,
	UNKNOWN,
};

enum class ResolverType {
	NONE,
	FREESTAND,
	LOGIC,
	ANIM,
	BRUTEFORCE
};

struct ResolverData_t {
	R_PlayerState player_state;
	R_AntiAimType antiaim_type;
	ResolverType resolver_type;

	AnimationLayer animlayers[3][13];

	float delta_positive = 0.f;
	float delta_negative = 0.f;
	float delta_center = 0.f;

	int side = 0;
};

struct BruteForceData_t {
	bool use = false;
	int current_side = 0;
	float last_shot = 0.f;

	void reset() {
		use = false;
		current_side = 0;
		last_shot = 0.f;
	}
};

class CResolver {
	BruteForceData_t brute_force_data[64];
public:
	CResolver() {
		for (int i = 0; i < 64; ++i) {
			brute_force_data[i].reset();
		}
	}	

	void			Reset(CBasePlayer* pl = nullptr);

	R_PlayerState	DetectPlayerState(CBasePlayer* player, AnimationLayer* animlayers);
	R_AntiAimType	DetectAntiAim(CBasePlayer* player, const std::deque<LagRecord>& records);

	void			SetupResolverLayers(CBasePlayer* player, LagRecord* record);

	void			DetectFreestand(CBasePlayer* player, LagRecord* record, const std::deque<LagRecord>& records);

	void			Apply(LagRecord* record);
	void			Run(CBasePlayer* player, LagRecord* record, std::deque<LagRecord>& records);

	void			OnMiss(CBasePlayer* player, LagRecord* record);
	void			OnHit(CBasePlayer* player, LagRecord* record);
};

extern CResolver* Resolver;