#include "Vector.h"

#include "../../Utils/Math.h"

float Vector::Q_Length() const {
	return Math::Q_sqrt(x * x + y * y + z * z);
}

float Vector::Q_Length2D() const {
	return Math::Q_sqrt(x * x + y * y);
}

Vector Vector::Q_Normalized() const {
	float len = Q_Length();
	if (len > 0.f)
		return Vector(x, y, z) / len;
	return Vector(0, 0, 0);
}

float Vector::Q_Normalize() {
	float len = Q_Length();
	if (len > 0.f) {
		x /= len;
		y /= len;
		z /= len;
	}
	return len;
}