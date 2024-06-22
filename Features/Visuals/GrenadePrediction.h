#pragma once

#include "../../SDK/Misc/CBaseEntity.h"
#include "../../SDK/Misc/CBasePlayer.h"
#include "../../SDK/Misc/CBaseCombatWeapon.h"
#include <vector>

class GrenadePrediction {
private:
	CNewParticleEffect* pMolotovParticle = nullptr;
	Vector lerped_velocity;

protected:
	int nTick = 0;
	int m_nLastUpdateTick;
	int nextThinkTick = 0.f;
	float flDetonateTime = 60.f;
	std::vector<Vector> pathPoints;
	std::vector<Vector> collisionPoints;
	int weaponId = 0;
	bool detonate;
	Vector vecVelocity;
	Vector vecOrigin;
	Vector vecDetonate;
	bool runningPrediction;
	float flThrowTime;
	float flExpireTime;
	int nCollisionGroup;
	CBasePlayer* owner;
	CBaseEntity* pLastHitEntity;
	int nBouncesCount;

public:
	static void PrecacheParticles();

	void Start();
	void Draw();
	void Predict(const Vector& orign, const Vector& velocity, float throwTime, int offset);
	bool PhysicsSimulate();
	void Think();
	void TraceHull(Vector& src, Vector& end, uint32_t mask, void* ignore, int collisionGroup, CGameTrace& trace);
	void TraceLine(Vector& src, Vector& end, uint32_t mask, void* ignore, int collisionGroup, CGameTrace& trace);
	void ClearTrace(CGameTrace& trace);
	void PhysicsTraceEntity(Vector& vecSrc, Vector& vecDst, uint32_t nMask, CGameTrace& pTrace);
	void PhysicsPushEntity(Vector& vecPush, CGameTrace& pTrace);
	void PerformFlyCollisionResolution(CGameTrace& pTrace);
	int  CalcDamage(Vector pos, CBasePlayer* target);
};

class GrenadeWarning : public GrenadePrediction {
public:
	void Warning(CBaseGrenade* entity, int weapId);
};

inline float CSGO_Armor(float flDamage, int ArmorValue) {
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

float CalculateThrowYaw(const Vector& wish_dir, const Vector& vel, float throw_velocity, float throw_strength);
float CalculateThrowPitch(const Vector& wish_dir, float wish_z_vel, const Vector& vel, float throw_velocity, float throw_strength);