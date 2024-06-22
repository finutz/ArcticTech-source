#pragma once

#include "../Misc/Vector.h"
#include "../Memory.h"

class CNewParticleEffect {
public:
	void StopEmission(bool bInfiniteOnly = false, bool bRemoveAllParticles = false, bool bWakeOnStop = false, bool bPlayEndCap = false) {
		static auto stopEmission = reinterpret_cast<void(__thiscall*)(CNewParticleEffect*, bool, bool, bool, bool)>(Utils::PatternScan("client.dll", "55 8B EC 56 8B F1 F6 86 ? ? ? ? ? 0F 84 ? ? ? ? 83 BE ? ? ? ? ? 0F 84 ? ? ? ? A1"));

		stopEmission(this, bInfiniteOnly, bRemoveAllParticles, bWakeOnStop, bPlayEndCap);
	}

	void SetRemoveFlag() {
		*(unsigned char*)(this + 944) |= 2u;
	}

	Vector& m_vecAggregationCenter() {
		static int offset = *reinterpret_cast<int*>(Utils::PatternScan("client.dll", "89 87 ? ? ? ? 8B 41 04 89 87 ? ? ? ? 8B 41 08 89 87 ? ? ? ? 80 A7 ? ? ? ? ? 8B C7 C7 87 ? ? ? ? ? ? ? ? 5F", 0x2));

		return *(Vector*)((uintptr_t)this + offset);
	}

	void SetSortOrigin(Vector origin) {
		static auto setSortOrigin = *reinterpret_cast<void(__thiscall*)(CNewParticleEffect*, const Vector&)>(Memory->ToAbsolute((uintptr_t)Utils::PatternScan("client.dll", "E8 ? ? ? ? 8B 4D FC 56 6A 00 E8 ? ? ? ? 8B 4D FC 8D 46 0C 50 6A 01 E8 ? ? ? ? 8D 45 E8", 0x1)));

		setSortOrigin(this, origin);
	}

	void SetControlPoint(int whichPoint, Vector origin) {
		static auto setControlPoint = *reinterpret_cast<void(__thiscall*)(CNewParticleEffect*, int, const Vector&)>(Utils::PatternScan("client.dll", "55 8B EC 53 8B 5D 0C 56 8B F1 F6 86 ? ? ? ? ? 0F 84"));

		setControlPoint(this, whichPoint, origin);
	}

	void SetOrigin(const Vector& origin, int maxPoints = 2) {
		m_vecAggregationCenter() = origin;
		SetSortOrigin(origin);

		for (int point = 0; point < maxPoints; point++)
			SetControlPoint(point, origin);
	}

	void Stop() {
		StopEmission();
		SetRemoveFlag();
	}
};

namespace IEFFECTS {
	inline bool bCaptureEffect = false;
	inline CNewParticleEffect* pCapturedEffect;
}

class IEffects
{
public:
	virtual ~IEffects() {};

	virtual void Beam(const Vector& Start, const Vector& End, int nModelIndex,
		int nHaloIndex, unsigned char frameStart, unsigned char frameRate,
		float flLife, unsigned char width, unsigned char endWidth, unsigned char fadeLength,
		unsigned char noise, unsigned char red, unsigned char green,
		unsigned char blue, unsigned char brightness, unsigned char speed) = 0;

	//-----------------------------------------------------------------------------
	// Purpose: Emits smoke sprites.
	// Input  : origin - Where to emit the sprites.
	//			scale - Sprite scale * 10.
	//			framerate - Framerate at which to animate the smoke sprites.
	//-----------------------------------------------------------------------------
	virtual void Smoke(const Vector& origin, int modelIndex, float scale, float framerate) = 0;

	virtual void Sparks(const Vector& position, int nMagnitude = 1, int nTrailLength = 1, const Vector* pvecDir = NULL) = 0;

	virtual void Dust(const Vector& pos, const Vector& dir, float size, float speed) = 0;

	virtual void MuzzleFlash(const Vector& vecOrigin, const Vector& vecAngles, float flScale, int iType) = 0;

	// like ricochet, but no sound
	virtual void MetalSparks(const Vector& position, const Vector& direction) = 0;

	virtual void EnergySplash(const Vector& position, const Vector& direction, bool bExplosive = false) = 0;

	virtual void Ricochet(const Vector& position, const Vector& direction) = 0;

	// FIXME: Should these methods remain in this interface? Or go in some 
	// other client-server neutral interface?
	virtual float Time() = 0;
	virtual bool IsServer() = 0;

	// Used by the playback system to suppress sounds
	virtual void SuppressEffectsSounds(bool bSuppress) = 0;

	CNewParticleEffect* DispatchParticleEffect(const char* particleName, Vector vecOrigin, QAngle vecAngles, CBaseEntity* pEntity = nullptr) {
		static auto dispatchParticleEffect = reinterpret_cast<void(__fastcall*)(const char*, Vector, QAngle, CBaseEntity*)>(Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 85 F6 74 25 8B 0D"));

		IEFFECTS::bCaptureEffect = true;
		dispatchParticleEffect(particleName, vecOrigin, vecAngles, pEntity);
		IEFFECTS::bCaptureEffect = false;

		return IEFFECTS::pCapturedEffect;
	}

	int PrecacheParticleSystem(const char* particleName);
};