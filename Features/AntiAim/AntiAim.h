#pragma once
#include <Windows.h>
#include <cstdint>

class CUserCmd;
class CBasePlayer;

struct LuaAntiAim_t {
	enum EOverrideBits {
		OverridePitch = (1 << 0),
		OverrideYaw = (1 << 1),
		OverrideYawOffset = (1 << 2),
		OverrideFakeLag = (1 << 3),
		OverrideDesync = (1 << 4),
		OverrideDesyncAngle = (1 << 5),
		OverrideDesyncSide = (1 << 6),
	};

	uint32_t override_bits = 0;

	float pitch = 0.f;
	float yaw = 0.f;
	float yaw_offset = 0.f;
	int fakelag = 0;
	bool desync = false;
	float desync_angle = 0.f;
	int desync_side = 0;

	void override_pitch(float p) {
		override_bits |= OverridePitch;
		pitch = p;
	}

	void override_yaw(float y) {
		override_bits |= OverrideYaw;
		yaw = y;
	}

	void override_yaw_offset(float yo) {
		override_bits |= OverrideYawOffset;
		yaw_offset = yo;
	}

	void override_fakelag(int f) {
		override_bits |= OverrideFakeLag;
		fakelag = f;
	}

	void override_desync(bool d) {
		override_bits |= OverrideDesync;
		desync = d;
	}

	void override_desync_angle(float l) {
		override_bits |= OverrideDesyncAngle;
		desync_angle = l;
	}

	void override_desync_side(int s) {
		override_bits |= OverrideDesyncSide;
		desync_side = s;
	}

	void reset() {
		override_bits = 0;
		pitch = 0.f;
		yaw = 0.f;
		yaw_offset = 0.f;
		fakelag = 0.f;
		desync = false;
		desync_angle = 0.f;
		desync_side = 0;
	}
};

class CAntiAim {
	int manualAngleState = 0;
	CBasePlayer* target;

	// Anti aim builder
	float pitch = 0.f;
public:
	float base_yaw = 0.f;
private:
	float yaw_offset = 0.f;

	int fakelag_limit = 0;
	int fakelag = 0;

	bool desync = false;
	float desync_limit = 0.f;
	int desync_side = 0;
public:
	bool jitter;
	float realAngle;
	bool desyncing = false;
	bool freezetime = false;

	LuaAntiAim_t lua_override;

	void LegMovement();
	void FakeLag();
	void Angles();
	void Desync();
	void SlowWalk();
	void FakeDuck();
	void JitterMove();

	bool IsPeeking();

	int DesyncFreestand();
	void OnKeyPressed(WPARAM key);
	CBasePlayer* GetNearestTarget();
	float AtTargets();
};

extern CAntiAim* AntiAim;