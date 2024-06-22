#include "KeyValues.h"
#include "../../Utils/Utils.h"

KeyValues::KeyValues(const char* name) {
	static auto constructor = reinterpret_cast<KeyValues*(__thiscall*)(KeyValues*, const char*, int, int)>(Utils::PatternScan("client.dll", "55 8B EC 56 8B F1 33 C0 8B 4D 0C 81 26 ? ? ? ? 89 46 10 8A 45 10 C6 46 03 00 89 4E 14 88 46 18 C7 46 ? ? ? ? ? C7 46"));

	constructor(this, name, 0, 0);
}

KeyValues* KeyValues::FindKey(const char* key, bool create) {
	static auto findKey = reinterpret_cast<KeyValues*(__thiscall*)(KeyValues*, const char*, bool)>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 1C 53 8B D9 85 DB 75 09 33 C0 5B 8B E5 5D C2 08 00"));

	return findKey(this, key, create);
}

void KeyValues::SetStringValue(const char* value) {
	static auto setStringValue = reinterpret_cast<void(__thiscall*)(KeyValues*, const char*)>(Utils::PatternScan("client.dll", "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01 FF 77 04"));

	return setStringValue(this, value);
}

void KeyValues::SetString(const char* key, const char* value) {
	if (KeyValues* dat = FindKey(key, true))
	{
		dat->SetStringValue(value);
	}
}

void KeyValues::SetInt(const char* keyName, int value)
{
	KeyValues* dat = FindKey(keyName, true);

	if (dat)
	{
		dat->m_iValue = value;
		dat->m_iDataType = TYPE_INT;
	}
}

void KeyValues::SetFloat(const char* keyName, float value)
{
	KeyValues* dat = FindKey(keyName, true);

	if (dat)
	{
		dat->m_flValue = value;
		dat->m_iDataType = TYPE_FLOAT;
	}
}