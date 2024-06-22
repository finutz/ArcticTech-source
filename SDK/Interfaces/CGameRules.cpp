#include "CGameRules.h"

CGameRules* GameRules() {
	static CGameRules** pp_GameRules = *(CGameRules***)(Utils::PatternScan("client.dll", "8B 0D ? ? ? ? 85 C9 0F 84 ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0", 0x2));
	return *pp_GameRules;
}