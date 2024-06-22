#pragma once
#include "ESP.h"
#include "../../SDK/Globals.h"

class CKillFeed
{
	template <class C>
	C* FindHudElement(const char* szName)
	{
		static auto fn = *reinterpret_cast<DWORD**>(Utils::PatternScan(("client.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"), 0x1));
		static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utils::PatternScan(("client.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
		return (C*)find_hud_element(fn, szName);
	}

	DWORD* DeathNotice = nullptr;
public:
	void Instance();

	bool ClearDeathNotice = false;
};

extern CKillFeed* KillFeed;