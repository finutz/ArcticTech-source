#pragma once
#include <Windows.h>

struct QAngle {
	float pitch, yaw, roll;

	QAngle() {
		pitch = 0;
		yaw = 0;
		roll = 0;
	}

	QAngle(float pitch_, float yaw_) {
		pitch = pitch_;
		yaw = yaw_;
		roll = 0.f;
	}

	QAngle(float pitch_, float yaw_, float roll_) {
		pitch = pitch_;
		yaw = yaw_;
		roll = roll_;
	}

	QAngle operator-(QAngle other) const {
		return QAngle(pitch - other.pitch, yaw - other.yaw, roll - other.roll);
	}

	QAngle operator*(float other) const {
		return QAngle(pitch * other, yaw * other, roll * other);
	}

	void Normalize(bool normalizeRoll = false) {
		while (yaw > 180) yaw -= 360;
		while (yaw < -180) yaw += 360;
		if (pitch > 89) pitch = 89;
		if (pitch < -89) pitch = -89;

		if (normalizeRoll)
			roll = max(min(roll, 50.f), -50.f);
	}
};