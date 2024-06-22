#pragma once

#include "../SDK/Misc/QAngle.h"
#include "../SDK/Misc/Vector.h"
#include "../SDK/Misc/Matrix.h"


#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 
#define DEG2RAD(deg) ((deg) * (PI_F / 180.f))
#define RAD2DEG(rad) ((rad) / (PI_F / 180.f))

namespace Math {
	inline float Q_sqrt(const float number) {
		const unsigned int i = (*(unsigned int*)&number + 0x3f800000) >> 1;
		const float approx = *(float*)&i;
		return (approx + number / approx) * 0.5f;
	}

	inline float RemapVal(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return (val - B) >= 0 ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	}

	float			Lerp(float a, float b, float perc);
	float			AngleNormalize(float angle);
	float           NormalizeYaw(float f);
	float			AngleNormalizePositive(float angle);
	float			AngleDiff(float next, float cur);
	float			AngleToPositive(float angle);
	void			AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up);
	void			AngleVectors(const QAngle& angles, Vector& forward);
	Vector			AngleVectors(const QAngle& angles);
	QAngle			VectorAngles(const Vector& vec);
	QAngle			VectorAngles_p(const Vector& vec);
	void			VectorTransform(const Vector& in, const matrix3x4_t& matrix, Vector* out);
	Vector			VectorTransform(const Vector& in, const matrix3x4_t& matrix);
	Vector			VectorRotate(const Vector& in, const matrix3x4_t& matrix);
	Vector			VectorRotate(const Vector& in, const QAngle& rotate);
}