#include "../../Utils/VitualFunction.h"

class IInputSystem {
public:
	void EnableInput(bool enable) {
		CallVFunction<void(__thiscall*)(void*, bool)>(this, 11)(this, enable);
	}
};