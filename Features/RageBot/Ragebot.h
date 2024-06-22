#pragma once
#include "../../SDK/Globals.h"
#include "../../SDK/Misc/Studio.h"
#include "LagCompensation.h"

#include <condition_variable>
#include <queue>
#include <shared_mutex>
#include <vector>

#define MAX_RAGEBOT_THREADS 8

inline int GetRagebotThreads() {
	int cpu_threads = std::thread::hardware_concurrency();
	if (cpu_threads <= 4)
		return 1;

	int threads = cpu_threads / 2;
	if (threads > MAX_RAGEBOT_THREADS)
		threads = MAX_RAGEBOT_THREADS;

	return threads;
}

class CUserCmd;

struct ScannedPoint_t {
	LagRecord* record = nullptr;
	Vector point;
	int hitbox = -1;
	int priority = 0;
	float damage = 0.f;
	bool safe_point = false;
	Vector impacts[5];
	int num_impacts = 0;
};

struct ScannedTarget_t {
	CBasePlayer* player = nullptr;
	std::vector<ScannedPoint_t> points;
	ScannedPoint_t best_point;
	QAngle angle;
	float hitchance = 0.f;
	float minimum_damage = 0.f;
};

struct AimPoint_t {
	Vector point;
	int hitbox = 0;
	bool multipoint = false;
};

class CRagebot {
private:
	struct SpreadValues_t {
		float a;
		float bcos;
		float bsin;
		float c;
		float dcos;
		float dsin;
	};

	SpreadValues_t spread_values[100];

	weapon_settings_t settings;
	bool doubletap_stop = false;
	float doubletap_stop_speed = 0.f;

	float last_frametime_check = 0.f;
	bool frametime_issues = false;

	CBasePlayer* last_target = nullptr;
	int last_target_shot = 0;

	// multithreading part
	std::atomic<bool> remove_threads = false;
	int inited_threads = 0;
	std::atomic<int> selected_points = 0;
	std::atomic<int> scanned_points = 0;
	HANDLE threads[MAX_RAGEBOT_THREADS];
	std::mutex scan_mutex{};
	std::mutex result_mutex{};
	std::mutex completed_mutex{};
	std::condition_variable scan_condition;
	std::condition_variable result_condition;

	LagRecord* current_record;
	std::queue<AimPoint_t> thread_work;
	ScannedTarget_t* result_target;

	std::vector<ScannedTarget_t> scanned_targets;
	std::array<int, 64> delayed_ticks;

	inline bool hitbox_enabled(int hitbox) {
		switch (hitbox)
		{
		case HITBOX_HEAD:
			return settings.hitboxes->get(0);
		case HITBOX_STOMACH:
			return settings.hitboxes->get(2);
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return settings.hitboxes->get(1);
		case HITBOX_RIGHT_THIGH:
		case HITBOX_LEFT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_LEFT_CALF:
			return settings.hitboxes->get(4);
		case HITBOX_RIGHT_FOOT:
		case HITBOX_LEFT_FOOT:
			return settings.hitboxes->get(5);
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_LEFT_UPPER_ARM:
			return settings.hitboxes->get(3);
		default:
			return false;
		}
	}

	inline bool multipoints_enabled(int hitbox) {
		switch (hitbox)
		{
		case HITBOX_HEAD:
			return settings.multipoints->get(0);
		case HITBOX_STOMACH:
			return settings.multipoints->get(2);
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return settings.multipoints->get(1);
		default:
			return false;
		}
	}

public:
	void				CalcSpreadValues();
	void				AutoStop();
	float				CalcHitchance(QAngle angles, LagRecord* target, int hitbox);
	float				FastHitchance(LagRecord* target, float inaccuracy = -1.f, int hitbox_radius = 5); // fast hitchance approximation base on inaccuracy angle and distance
	float				CalcMinDamage(CBasePlayer* target);
	weapon_settings_t	GetWeaponSettings(int weaponId);
	bool				IsArmored(int hitbox);

	void				AddPoint(const AimPoint_t& point);
	void				GetMultipoints(LagRecord* record, int hitbox, float scale);
	std::queue<LagRecord*> SelectRecords(CBasePlayer* player);
	void				SelectPoints(LagRecord* record);
	bool				CompareRecords(LagRecord* a, LagRecord* b);

	void				CreateThreads();
	void				TerminateThreads();
	void				SelectBestPoint(ScannedTarget_t* target);
	void				ScanTargets();
	static uintptr_t	ThreadScan(int threadId);
	void				ScanTarget(CBasePlayer* target);

	void				Run();
	void				Zeusbot();
	void				Knifebot();
	void				AutoRevolver();
	void				DormantAimbot();
};

extern CRagebot* Ragebot;