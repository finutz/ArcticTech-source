#pragma once
#include "../../SDK/Interfaces.h"
#include "../../SDK/Misc/CBasePlayer.h"
#include "../../SDK/NetMessages.h"

class CSVCMsg_VoiceData;

struct ESPInfo_t {
	CBasePlayer*	m_pEnt = nullptr;
	Vector			m_vecOrigin;
	Vector2			m_BoundingBox[2];
	int				m_nFakeDuckTicks = 0;
	int				m_nHealth;
	bool			m_bFakeDuck = 0;
	bool			m_bIsScoped = false;
	bool			m_bExploiting = false;
	bool			m_bBreakingLagComp = false;
	bool			m_bDormant = false;
	bool			m_bValid = false;
	float			m_flAlpha = 0.f;
	float			m_flLastUpdateTime = 0.f;
	int				m_iActiveWeapon;

	void reset() {
		m_pEnt = nullptr;
		m_vecOrigin = Vector();
		m_nHealth = 0;
		m_flLastUpdateTime = 0.f;
		m_flAlpha = 0.f;
		m_bValid = false;
	}
};

enum SharedESPFlags {
	Shared_Scoped = (1 << 0),
	Shared_BreakLC = (1 << 1),
	Shared_Exploiting = (1 << 2),
	Shared_FakeDuck = (1 << 3),
};

struct VectorSmall {
	int16_t x = 0, y = 0, z = 0;

	VectorSmall() {}

	VectorSmall(Vector vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
};

struct SharedESP_t {
	int		m_iPlayer;
	short	m_ActiveWeapon;
	short	m_iHealth;
	VectorSmall	m_vecOrigin;
};

static_assert(sizeof(SharedESP_t) == sizeof(SharedVoiceData_t));

extern ESPInfo_t ESPInfo[64];

namespace ESP {
	void		ProcessSound(const SoundInfo_t& sound);
	void		RegisterCallback();

	void		ProcessSharedESP(const SharedVoiceData_t* data);
	void		UpdatePlayer(int id);
	void		Draw();
	void		IconDisplay( CBasePlayer* pLocal, int Level );
	void		DrawPlayer(int id);
	void		DrawBox(ESPInfo_t info);
	void		DrawHealth(ESPInfo_t info);
	void		DrawName(ESPInfo_t info);
	void		DrawFlags(ESPInfo_t info);
	void		DrawWeapon(ESPInfo_t info);

	void		DrawGrenades();

	void		AddHitmarker(const Vector& position);
	void		AddDamageMarker(const Vector& postion, int damage);
	void		RenderMarkers();
}