#include "Animation.h"
#include "../SDK/Interfaces.h"
#include <algorithm>

float Animation::GetValue() {
	return animValue;
}

float Animation::UpdateAnimation(bool newValue) {
	if (newValue != bValue) {
		if (animFraction != 1.f) {
			this->changeTime =  GlobalVars->realtime - (1 - animFraction) * speed;
			this->endTime = changeTime + speed;
		}
		else {
			this->changeTime = GlobalVars->realtime;
			this->endTime = GlobalVars->realtime + speed;
		}
		bValue = newValue;
	}

	animFraction = std::clamp((GlobalVars->realtime - changeTime) / speed, 0.f, 1.f);
	animValue = newValue ? animFraction : 1.f - animFraction;

	return animValue;
}

float Animation::Interpolate(float value, int speed) {
	animValue = animValue + (value - animValue) * GlobalVars->frametime * speed;
	return animValue;
}