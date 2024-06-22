#pragma once
#include "../Misc/Vector.h"
#include "../Misc/QAngle.h"
#include "../Misc/UtlVector.h"
#include "../Misc/Color.h"

#include "../../Utils/Utils.h"

class CBaseEntity;

class GlowObjectDefinition_t
{
public:
    GlowObjectDefinition_t() { memset(this, 0, sizeof(*this)); }

    int m_nNextFreeSlot;
	CBaseEntity* m_pEntity;
	Vector m_vGlowColor;
	float m_flGlowAlpha;

	bool m_bGlowAlphaCappedByRenderAlpha;
	float m_flGlowAlphaFunctionOfMaxVelocity;
	float m_flGlowAlphaMax;
	float m_flGlowPulseOverdrive;
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloomRender;
	int m_nFullBloomStencilTestValue; // only render full bloom objects if stencil is equal to this value (value of -1 implies no stencil test)
	int m_nRenderStyle;
	int m_nSplitScreenSlot;

    bool IsUnused() const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

    static const int END_OF_FREE_LIST = -1;
    static const int ENTRY_IN_USE = -2;
};

class CGlowObjectManager {
public:
	CUtlVector<GlowObjectDefinition_t> m_GlowObjects;
	int m_nFirstFreeSlot;

	struct GlowBoxDefinition_t
	{
		Vector m_vPosition;
		QAngle m_angOrientation;
		Vector m_vMins;
		Vector m_vMaxs;
		float m_flBirthTimeIndex;
		float m_flTerminationTimeIndex; //when to die
		Color m_colColor;
	};

	CUtlVector< GlowBoxDefinition_t > m_GlowBoxDefinitions;

	int AddGlowBox(Vector vecOrigin, QAngle angOrientation, Vector mins, Vector maxs, Color colColor, float flLifetime) {
		static auto addGlowBox = reinterpret_cast<int(__thiscall*)(CGlowObjectManager*, Vector, QAngle, Vector, Vector, Color, float)>(Utils::PatternScan("client.dll", "55 8B EC 53 56 8D 59"));

		return addGlowBox(this, vecOrigin, angOrientation, mins, maxs, colColor, flLifetime);
	}
};