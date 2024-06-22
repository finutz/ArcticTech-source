#pragma once

#define interpolate(start, end, st) ((start) + ((end) - (start)) * (st))

class Animation {
private:
	float animValue;
	float animFraction = 1.f;
	int iValue;
	bool bValue;
	int prevValue;
	float changeTime;
	float endTime;
public:
	float	GetValue();
	float	UpdateAnimation(bool newValue);
	float	UpdateAnimation(int newValue);
	float	Interpolate(float value, int speed=8);

	int type;
	float speed = 0.2f;
};