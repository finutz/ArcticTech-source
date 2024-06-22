#include "PreserveKillfeed.h"

void CKillFeed::Instance()
{
	//if (!Cheat.LocalPlayer->IsAlive())
	//	return;

	//if (!DeathNotice)
	//	DeathNotice = this->FindHudElement<DWORD>("CCSGO_HudDeathNotice");

	//if (DeathNotice != nullptr)
	//{
	//	auto local_death_notice = (float*)((uintptr_t)DeathNotice + 0x50);

	//	if (local_death_notice)
	//		*local_death_notice = config.visuals.effects.preserve_killfeed->get() ? FLT_MAX : 1.5f;

	//	if (ClearDeathNotice)
	//	{
	//		ClearDeathNotice = false;

	//		using Fn = void(__thiscall*)(uintptr_t);
	//		static auto clear_notices = (Fn)Utils::PatternScan("client.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58");

	//		clear_notices((uintptr_t)DeathNotice - 0x14);
	//	}
	//}
}

CKillFeed* KillFeed = new CKillFeed;