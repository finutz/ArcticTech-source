#pragma once

#include "../../Utils/Utils.h"

class CGameRules {
	char pad__[0x20];
public:
	bool m_bFreezePeriod;
	bool m_bWarmupPeriod;
	float m_fWarmupPeriodEnd;
	float m_fWarmupPeriodStart;

	bool IsFreezePeriod() {
		return m_bFreezePeriod;
	}
};

CGameRules* GameRules();