#pragma once

#include <vector>
#include <string>
#include <cstdint>

#define NET_ARCTIC_CODE 1099264u

enum ArcticMsg_Type : unsigned char {
	ArcticMsg_SharedESP
};

enum VoiceData_Has {
	Data = 1u,
	Xuid = 2u,
	Format = 4u,
	SequenceBytes = 8u,
	SectionNumber = 10u,
	UncompressedSampleOffset = 20u
};

class CCLCMsg_VoiceData_t {
	char data[0x58];
public:
	CCLCMsg_VoiceData_t();

	void set_data(void* data, int length);

	uint32_t& has_bits() {
		return *(uint32_t*)((uintptr_t)this + 0x34);
	}

	int& format() {
		return *(int*)((uintptr_t)this + 0x20);
	}

	uint64_t& xuid() {
		return *(uint64_t*)((uintptr_t)this + 0x18);
	}

	uint32_t& xuid_low() {
		return *(uint32_t*)((uintptr_t)this + 0x18);
	}

	uint32_t& xuid_high() {
		return *(uint32_t*)((uintptr_t)this + 0x1C);
	}

	int& sequence_bytes() {
		return *(int*)((uintptr_t)this + 0x24);
	}

	uint32_t& section_number() {
		return *(uint32_t*)((uintptr_t)this + 0x28);
	}

	uint32_t& uncompressed_sample_offset() {
		return *(uint32_t*)((uintptr_t)this + 0x2C);
	}
};

class CSVCMsg_VoiceData {
	char pad_00[8];

public:
	int client;
	int audible_mask;
	union {
		uint64_t xuid;
		struct {
			uint32_t xuid_low;
			uint32_t xuid_high;
		};
	};
	std::string* voice_data;
	bool proximity;
	bool caster;
	char pad_30[2];
	int format;
	int sequence_bytes;
	uint32_t section_number;
	uint32_t uncompressed_sample_offset;

	std::string get_voice_data() {
		return *voice_data;
	}
};

class CBasePlayer;
struct CSVCMsg_VoiceData_Lua {
	CBasePlayer* client;
	int audible_mask;
	uint64_t xuid;
	uint32_t xuid_low;
	uint32_t xuid_high;
	std::string* voice_data;
	bool proximity;
	bool caster;
	int format;
	int sequence_bytes;
	uint32_t section_number;
	uint32_t uncompressed_sample_offset;

	std::string get_voice_data() {
		return *voice_data;
	}
};

struct SharedVoiceData_t {
	uint32_t xuid_high;
	int sequence_bytes;
	uint32_t section_number;
	uint32_t uncompressed_sample_offset;
};

typedef void(*tRecieveVoiceDataCallback)(const CSVCMsg_VoiceData&);
typedef void(*tRecieveArcticDataCallback)(const SharedVoiceData_t*);

class CNetMessages {
	std::vector<tRecieveVoiceDataCallback> m_voiceDataCallbacks;
	std::vector<tRecieveArcticDataCallback> m_arcticDataCallbacks;

public:

	void SendNetMessage(SharedVoiceData_t* data);
	void AddVoiceDataCallback(tRecieveVoiceDataCallback callback) {
		m_voiceDataCallbacks.emplace_back(callback);
	}
	void AddArcticDataCallback(tRecieveArcticDataCallback callback) {
		m_arcticDataCallbacks.emplace_back(callback);
	}
	void RemoveVoiceDataCallback(tRecieveVoiceDataCallback callback) {
		for (auto it = m_voiceDataCallbacks.begin(); it != m_voiceDataCallbacks.end(); it++) {
			if (*it == callback) {
				m_voiceDataCallbacks.erase(it);
				return;
			}
		}
	}
	void RemoveArcticDataCallback(tRecieveArcticDataCallback callback) {
		for (auto it = m_arcticDataCallbacks.begin(); it != m_arcticDataCallbacks.end(); it++) {
			if (*it == callback) {
				m_arcticDataCallbacks.erase(it);
				return;
			}
		}
	}
	bool OnVoiceDataRecieved(const CSVCMsg_VoiceData& msg);
};

extern CNetMessages* NetMessages;