#pragma once
#include "../../Utils/Animation.h"
#include "../../SDK/Misc/Vector.h"

class CUserCmd;

class CAutoPeek {
public:
	bool returning;
	Vector position;
	Animation circleAnimation;

	void CreateMove();
	void Draw();
};

extern CAutoPeek* AutoPeek;