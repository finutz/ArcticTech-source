#pragma once

#include <vector>
#include <string>

#include "../../SDK/Misc/Vector.h"
#include "../../SDK/Misc/QAngle.h"


class CBasePlayer;
class IGameEvent;
struct LagRecord;

struct PlayerHurt_t {
	CBasePlayer* victim;
	int damagegroup = -1;
};

struct RegisteredShot_t {
	// client info
	Vector client_shoot_pos;
	Vector target_pos;
	QAngle client_angle;
	int shot_tick;
	int wanted_damage;
	int wanted_damagegroup;
	int hitchance = 0;
	int backtrack = 0;
	LagRecord* record;
	QAngle player_angle; // for building correct matrix
	bool safe_point = false;

	// acked info
	Vector shoot_pos;
	Vector end_pos;
	QAngle angle;
	int	ack_tick = 0;
	std::vector<Vector> impacts;
	int damage = 0;
	int	damagegroup = -1;
	Vector hit_point;
	int health = 0;

	bool unregistered = false;
	bool death = false;
	bool player_death = false;
	bool acked = false;
	std::string_view miss_reason = "none";
};

class CBaseCombatWeapon;
class CShotManager {
	std::vector<RegisteredShot_t>	m_RegisteredShots;

	float	m_fLastShotTime = 0.f;
	CBaseCombatWeapon* last_weapon = nullptr;

public:
	void	DetectUnregisteredShots();
	void	ProcessManualShot();
	void	OnNetUpdate();
	bool	OnEvent(IGameEvent* event);
	void	AddShot(const Vector& shoot_pos, const Vector& target_pos, int damage, int damagegroup, int hitchance, bool safe, LagRecord* record);
	void	Reset();
};

extern CShotManager* ShotManager;