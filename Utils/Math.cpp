#include "Math.h"

#include <DirectXMath.h>

float Math::Lerp(float a, float b, float perc) {
	return a + (b - a) * perc;
}

float Math::AngleNormalize(float angle) {
	angle = fmodf(angle, 360.0f);
	if (angle > 180)
	{
		angle -= 360;
	}
	if (angle < -180)
	{
		angle += 360;
	}
	return angle;
}
float Math::NormalizeYaw(float f)
{
	while (f < -180.0f)
		f += 360.0f;

	while (f > 180.0f)
		f -= 360.0f;

	return f;
}
float Math::AngleNormalizePositive(float angle) {
	angle = fmodf(angle, 360.0f);

	if (angle < 0.0f)
	{
		angle += 360.0f;
	}

	return angle;
}

float Math::AngleDiff(float next, float cur) {
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return delta;
}

float Math::AngleToPositive(float angle) {
	return angle > 0.f ? angle : 360.f + angle;
}

void Math::AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up) {
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.pitch));
	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.yaw));
	DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles.roll));

	forward.x = (cp * cy);
	forward.y = (cp * sy);
	forward.z = (-sp);
	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr * cy);
	right.z = (-1 * sr * cp);
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = (cr * cp);
}

void Math::AngleVectors(const QAngle& angles, Vector& forward)
{
	float	sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.pitch));
	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.yaw));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

Vector Math::AngleVectors(const QAngle& angles) {
	float	sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.pitch));
	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.yaw));

	return Vector(
		cp * cy,
		cp * sy,
		-sp
	);
}

QAngle Math::VectorAngles(const Vector& vec) {
	QAngle result;

	float hyp2d = Q_sqrt(vec.x * vec.x + vec.y * vec.y);

	result.pitch = -atanf(vec.z / hyp2d) * 57.295779513082f;
	result.yaw = atan2f(vec.y, vec.x) * 57.295779513082f;
	
	return result;
}

QAngle Math::VectorAngles_p(const Vector& vec) {
	QAngle result;

	float hyp2d = std::sqrtf(vec.x * vec.x + vec.y * vec.y);

	result.pitch = -std::atanf(vec.z / hyp2d) * 57.295779513082f;
	result.yaw = std::atan2f(vec.y, vec.x) * 57.295779513082f;

	return result;
}

void Math::VectorTransform(const Vector& in, const matrix3x4_t& matrix, Vector* out) {
	out->x = in.Dot(matrix[0]) + matrix[0][3];
	out->y = in.Dot(matrix[1]) + matrix[1][3];
	out->z = in.Dot(matrix[2]) + matrix[2][3];
}

Vector Math::VectorTransform(const Vector& in, const matrix3x4_t& matrix) {
	return Vector(
		in.Dot(matrix[0]) + matrix[0][3],
		in.Dot(matrix[1]) + matrix[1][3],
		in.Dot(matrix[2]) + matrix[2][3]
	);
}

Vector Math::VectorRotate(const Vector& in1, const matrix3x4_t& in2)
{
	return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
}
//--------------------------------------------------------------------------------
Vector Math::VectorRotate(const Vector& in1, const QAngle& in2)
{
	matrix3x4_t mat;
	mat.AngleMatrix(in2);
	return VectorRotate(in1, mat);
}