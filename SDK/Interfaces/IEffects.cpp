#include "../Interfaces.h"


int IEffects::PrecacheParticleSystem(const char* particleName) {
	static auto precacheParticleSystem = reinterpret_cast<void(__stdcall*)(int, const char*)>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 08 53 8B 5D 0C 56 57 85 DB 0F 84 ? ? ? ? 80 3B 00"));

	INetworkStringTable* stringTableParticleEffectNames = NetworkStringTableContainer->FindTable("ParticleEffectNames");

	if (!stringTableParticleEffectNames)
		return INVALID_STRING_INDEX;

	int index = stringTableParticleEffectNames->FindStringIndex(particleName);

	if (index != INVALID_STRING_INDEX)
		return index;

	index = stringTableParticleEffectNames->AddString(false, particleName);
	precacheParticleSystem(index, particleName);
	return index;
}