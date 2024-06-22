#pragma once

#include "../../Utils/NetVars.h"
#include "../../SDK/Misc/Vector.h"

#define NETVAR(func, type, table, netvar)								\
	type& func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar);	\
		return *(type*)(this + _##func);				\
	}	

#define PNETVAR(func, type, table, netvar)								\
	type* func##() {								\
		static int _##func = NetVars::GetNetVar(table, netvar);	\
		return (type*)((uintptr_t)this + _##func);				\
	}	


class CCSPlayerResource
{
public:
	NETVAR(m_bombsiteCenterA, Vector, "DT_CSPlayerResource", "m_bombsiteCenterA");
	NETVAR(m_bombsiteCenterB, Vector, "DT_CSPlayerResource", "m_bombsiteCenterB");
	NETVAR(m_iPlayerC4, int, "DT_CSPlayerResource", "m_iPlayerC4");
	PNETVAR(m_bAlive, bool, "DT_PlayerResource", "m_bAlive");
};