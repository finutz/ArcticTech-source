#include "IClientEntityList.h"

#include "../../Utils/Utils.h"


void* UTIL_GetServerPlayer(int playerIndex) {
	static auto UTIL_PlayerByIndex = reinterpret_cast<void*(__fastcall*)(int)>(Utils::PatternScan("server.dll", "85 C9 7E 32 A1 ? ? ? ?"));

	return UTIL_PlayerByIndex(playerIndex);
}