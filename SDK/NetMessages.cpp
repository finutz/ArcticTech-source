#include "NetMessages.h"

#include "../Utils/Utils.h"
#include "../Utils/Console.h"
#include "Memory.h"
#include "../Features/Lua/Bridge/Bridge.h"

#include "Globals.h"

CNetMessages* NetMessages = new CNetMessages;


CCLCMsg_VoiceData_t::CCLCMsg_VoiceData_t() {
	static auto init_fn = reinterpret_cast<CCLCMsg_VoiceData_t* (__thiscall*)(CCLCMsg_VoiceData_t*)>(Memory->ToAbsolute((uintptr_t)Utils::PatternScan("engine.dll", "E8 ? ? ? ? 56 8D 84 24 ? ? ? ? 50 8D 4C 24 28", 0x1)));

	init_fn(this);
}

void CCLCMsg_VoiceData_t::set_data(void* data, int length) {
	static auto set_data_fn = reinterpret_cast<void(__thiscall*)(void*, void*, size_t)>(Memory->ToAbsolute((uintptr_t)Utils::PatternScan("engine.dll", "E8 ? ? ? ? 83 4C 24 ? ? 83 7C 24", 0x1)));

	set_data_fn(reinterpret_cast<void*>((uintptr_t)this + 0x8), data, length);
}

void CNetMessages::SendNetMessage(SharedVoiceData_t* data) {
	CCLCMsg_VoiceData_t msg;

	msg.has_bits() = 63;
	msg.format() = 0;

	msg.xuid_low() = NET_ARCTIC_CODE;
	msg.xuid_high() = data->xuid_high;
	msg.section_number() = data->section_number;
	msg.sequence_bytes() = data->sequence_bytes;
	msg.uncompressed_sample_offset() = data->uncompressed_sample_offset;


	//player_info_t pinfo;
	//if (EngineClient->GetPlayerInfo(EngineClient->GetLocalPlayer(), &pinfo)) {
	//	msg.xuid() = pinfo.steamID64;
	//	msg.has_bits() |= VoiceData_Has::Xuid;
	//}

	INetChannel* netChan = ClientState->m_NetChannel;

	if (netChan) {
		netChan->SendNetMsg(&msg, false, true);
	}
}

bool CNetMessages::OnVoiceDataRecieved(const CSVCMsg_VoiceData& msg) {
	if (msg.client + 1 == EngineClient->GetLocalPlayer())
		return true;

	CSVCMsg_VoiceData_Lua lua_voice_data;
	lua_voice_data.client = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(msg.client + 1));
	lua_voice_data.audible_mask = msg.audible_mask;
	lua_voice_data.xuid = msg.xuid;
	lua_voice_data.xuid_low = msg.xuid_low;
	lua_voice_data.xuid_high = msg.xuid_high;
	lua_voice_data.voice_data = msg.voice_data;
	lua_voice_data.proximity = msg.proximity;
	lua_voice_data.caster = msg.caster;
	lua_voice_data.sequence_bytes = msg.sequence_bytes;
	lua_voice_data.section_number = msg.section_number;
	lua_voice_data.uncompressed_sample_offset = msg.uncompressed_sample_offset;
	lua_voice_data.format = msg.format;

	for (auto& func : Lua->hooks.getHooks(LUA_VOICE_DATA)) {
		func.func(lua_voice_data);
	}

	for (auto handler : m_voiceDataCallbacks)
		handler(msg);

	if (msg.xuid_low != NET_ARCTIC_CODE)
		return false;

#ifdef DEBUG_PRINT_VOICEDATA
	if (msg.voice_data->size() == 0) {
		CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(msg.client + 1));

		if (player)
			Console->Log(std::format("recieved msg from {} [format: {}] [xuid_low: {}] [xuid_high: {}] [seq: {}] [sect: {}] [uso: {}]", player->GetName(), msg.format, msg.xuid_low, msg.xuid_high, msg.sequence_bytes, msg.section_number, msg.uncompressed_sample_offset));
}
#endif

	player_info_t info;
	EngineClient->GetPlayerInfo(msg.client + 1, &info);

	SharedVoiceData_t* data = new SharedVoiceData_t;
	data->xuid_high = msg.xuid_high;
	data->section_number = msg.section_number;
	data->sequence_bytes = msg.sequence_bytes;
	data->uncompressed_sample_offset = msg.uncompressed_sample_offset;

	for (auto handler : m_arcticDataCallbacks)
		handler(data);

	delete data;
}