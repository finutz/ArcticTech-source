#pragma once
#include "../../SDK/Interfaces/IGameEvents.h"


class CEventListner : public IGameEventListener2 {
public:
	void FireGameEvent(IGameEvent* event);
	int GetEventDebugID();
	void Register();
	void Unregister();
};

extern CEventListner* EventListner;