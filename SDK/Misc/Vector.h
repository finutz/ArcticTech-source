#pragma once
#include <cmath>


class Vector2 {
public:
	float x, y;

	Vector2() {
		x = 0;
		y = 0;
	}

	Vector2(int _x, int _y) {
		x = _x;
		y = _y;
	}

	Vector2(float _x, float _y) {
		x = (int)_x;
		y = (int)_y;
	}

	Vector2(long _x, long _y) {
		x = (int)_x;
		y = (int)_y;
	}

	Vector2(float _x, int _y) {
		x = _x;
		y = _y;
	}

	Vector2(int _x, float _y) {
		x = _x;
		y = _y;
	}

	Vector2 operator+(const Vector2& o) const {
		return Vector2(x + o.x, y + o.y);
	}

	Vector2 operator-(const Vector2& o) const {
		return Vector2(x - o.x, y - o.y);
	}

	Vector2 operator*(const float o) const {
		return Vector2(x * o, y * o);
	}

	Vector2 operator/(const float o) const {
		return Vector2(x / o, y / o);
	}

	void operator+=(const Vector2& o) {
		x += o.x;
		y += o.y;
	}

	void operator-=(const Vector2& o) {
		x -= o.x;
		y -= o.y;
	}

	void operator*=(const Vector2& o) {
		x *= o.x;
		y *= o.y;
	}

	void operator/=(const Vector2& o) {
		x /= o.x;
		y /= o.y;
	}

	bool operator==(const Vector2& o) const {
		return (x == o.x && y == o.y);
	}

	bool operator!=(const Vector2& o) const {
		return (x != o.x || y != o.y);
	}

	bool Invalid() const {
		return x == -1 && y == -1;
	}
};


class Vector {
public:
	float x, y, z;

	Vector() {
		x = 0;
		y = 0;
		z = 0;
	}

	Vector(float _x, float _y) {
		x = _x;
		y = _y;
		z = 0;
	}

	Vector(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector(const float* xyz) {
		memcpy(this, xyz, 12);
	}

	Vector operator+(Vector o) const {
		return Vector(x + o.x, y + o.y, z + o.z);
	}

	Vector operator-(Vector o) const {
		return Vector(x - o.x, y - o.y, z - o.z);
	}

	Vector operator*(float o) const {
		return Vector(x * o, y * o, z * o);
	}

	Vector operator/(float o) const {
		return Vector(x / o, y / o, z / o);
	}

	void operator+=(Vector o) {
		x += o.x;
		y += o.y;
		z += o.z;
	}

	void operator-=(const Vector& o) {
		x -= o.x;
		y -= o.y;
		z -= o.z;
	}

	void operator*=(const Vector& o) {
		x *= o.x;
		y *= o.y;
		z *= o.z;
	}

	void operator/=(const Vector& o) {
		x /= o.x;
		y /= o.y;
		z /= o.z;
	}

	void operator*=(float o) {
		x *= o;
		y *= o;
		z *= o;
	}

	void operator/=(float o) {
		x /= o;
		y /= o;
		z /= o;
	}

	float& operator[](int i) {
		return ((float*)this)[i];
	}

	bool operator==(const Vector& o) const {
		return (x == o.x && y == o.y && z == o.z);
	}

	bool operator!=(const Vector& o) const {
		return (x != o.x || y != o.y || z != o.z);
	}

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
	{
		x = ix; y = iy; z = iz;
	}

	float LengthSqr() const {
		return x * x + y * y + z * z;
	}

	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float Q_Length() const;

	float Length2D() const {
		return std::sqrt(x * x + y * y);
	}

	float Q_Length2D() const;

	float Length2DSqr() const {
		return x * x + y * y;
	}

	Vector Normalized() {
		float len = Length();
		if (len > 0.f)
			return Vector(x, y, z) / len;
		else
			return Vector(0, 0, 0);
	}

	float DistTo(const Vector& vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}

	Vector Q_Normalized() const;

	float Normalize() {
		const float len = Length();
		if (len > 0.f) {
			x /= len;
			y /= len;
			z /= len;
		}
		return len;
	}

	float Q_Normalize();

	bool Zero() const {
		return x == 0 && y == 0 && z == 0;
	}

	float Dot(const Vector& vOther) const {
		return x * vOther.x + y * vOther.y + z * vOther.z;
	}

	Vector Cross(const Vector& other) const {
		return Vector(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	void Interpolate(const Vector& other, float frac) {
		*this += (other - *this) * frac;
	}

	Vector2 to_vec2() const {
		return Vector2(x, y);
	}
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	inline VectorAligned(void) {};
	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

public:
	explicit VectorAligned(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned& operator=(const Vector& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	VectorAligned& operator=(const VectorAligned& vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}
	float LengthSqr() {
		return std::sqrt((x * x, y * y, z * z));
	}
	float operator[](int index) const {
		switch (index) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		}
	}

	float w = 0.f;
};

struct Vector4D {
	float x, y, z, w;
};