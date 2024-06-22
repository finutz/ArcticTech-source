#pragma once
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"
#include "../../SDK/Interfaces.h"

class CBaseEntity;
class CUserCmd;

struct local_netvars_t {
	int command_number;
	int m_nTickBase;

	float m_flVelocityModifier;
	float m_flFallVelocity;
	float m_flDuckAmount;
	float m_flDuckSpeed;
	float m_flThirdpersonRecoil;

	QAngle m_aimPunchAngle;
	QAngle m_aimPunchAngleVel;
	Vector m_vecViewOffset;
	QAngle m_viewPunchAngle;
	Vector m_vecVelocity;
	Vector m_vecNetworkOrigin;

	bool filled;

	void reset()
	{
		command_number = 0;

		m_flVelocityModifier = 0.f;
		m_flFallVelocity = 0.f;
		m_flDuckAmount = 0.f;
		m_flDuckSpeed = 0.f;
		m_flThirdpersonRecoil = 0.f;

		filled = false;
	}
};

struct local_data_t {
	float m_flSpawnTime = 0.f;
	int m_nTickBase = 0;
	int shift_amount = 0;

	void init(const CUserCmd* cmd) {
		m_flSpawnTime = Cheat.LocalPlayer->m_flSpawnTime();
		m_nTickBase = Cheat.LocalPlayer->m_nTickBase();
		shift_amount = ctx.tickbase_shift;
	}
};

class CPrediction {
private:
	int* predictionRandomSeed;
	CBaseEntity** predictionEntity;
	CMoveData moveData = {};
	float flOldCurrentTime = 0.f;
	float flOldFrameTime = 0.f;
	int iOldTickcount = 0;
	bool bOldIsFirstPrediction = false;
	bool bOldInPrediction = false;
	local_data_t local_data[MULTIPLAYER_BACKUP];
	local_netvars_t local_netvars[MULTIPLAYER_BACKUP];

	float weaponInaccuracy = 0.f;
	float weaponSpread = 0.f;
	float weaponAccuracyPenality = 0.f;
	float weaponRecoilIndex = 0.f;

	bool has_prediction_errors = false;
public:
	float m_flNextPrimaryAttack = 0;
	int m_fFlags = 0;
	float m_fThrowTime = 0.f;
	Vector m_vecVelocity;
	Vector m_vecAbsVelocity;

	__forceinline float WeaponInaccuracy() { return weaponInaccuracy; };
	__forceinline float WeaponSpread() { return weaponSpread; };
	__forceinline float frametime() { return flOldFrameTime; };
	__forceinline float curtime() { return flOldCurrentTime; };
	__forceinline int tickcount() { return iOldTickcount; };
	__forceinline bool HasPredictionErrors() { return has_prediction_errors; };

	inline local_data_t& GetLocalData(int place) { return local_data[place % MULTIPLAYER_BACKUP]; };

	void StartCommand(CBasePlayer* player, CUserCmd* cmd);
	void RunPreThink(CBasePlayer* player);
	void RunThink(CBasePlayer* player);
	void BackupData();

	void Start(CUserCmd* cmd);
	void End();

	void StoreNetvars(int place);
	void RestoreNetvars(int place);

	void PatchAttackPacket(CUserCmd* cmd, bool restore);

	CPrediction() {
		predictionRandomSeed = *(int**)Utils::PatternScan("client.dll", "8B 47 40 A3", 0x4); // 0x10DA7244
		predictionEntity = *(CBaseEntity***)Utils::PatternScan("client.dll", "0F 5B C0 89 35", 0x5); // 0x1532D108
	}
};

class CNetData {
private:
	int m_nTickBase;
	Vector m_vecVelocity;
};

extern CPrediction* EnginePrediction;