#pragma once
#include "../Misc/Vector.h"

class CBaseEntity;

struct BeamInfo_t
{
	int			m_nType;

	// Entities
	CBaseEntity* m_pStartEnt;
	int			m_nStartAttachment;
	CBaseEntity* m_pEndEnt;
	int			m_nEndAttachment;

	// Points
	Vector		m_vecStart;
	Vector		m_vecEnd;

	int			m_nModelIndex;
	const char* m_pszModelName;

	int			m_nHaloIndex;
	const char* m_pszHaloName;
	float		m_flHaloScale;

	float		m_flLife;
	float		m_flWidth;
	float		m_flEndWidth;
	float		m_flFadeLength;
	float		m_flAmplitude;

	float		m_flBrightness;
	float		m_flSpeed;

	int			m_nStartFrame;
	float		m_flFrameRate;

	float		m_flRed;
	float		m_flGreen;
	float		m_flBlue;

	bool		m_bRenderable;

	int			m_nSegments;

	int			m_nFlags;

	// Rings
	Vector		m_vecCenter;
	float		m_flStartRadius;
	float		m_flEndRadius;

	BeamInfo_t()
	{
		m_nType = 0;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}
};

//-----------------------------------------------------------------------------
// Purpose: Declare client .dll beam entity interface
//-----------------------------------------------------------------------------

class ITraceFilter;
class C_Beam;
class Beam_t;

class IViewRenderBeams {
public:
	// Construction
public:
	IViewRenderBeams(void);
	virtual				~IViewRenderBeams(void) = 0;

	// Implement IViewRenderBeams
public:
	virtual	void		InitBeams(void) = 0;
	virtual	void		ShutdownBeams(void) = 0;
	virtual	void		ClearBeams(void) = 0;

	// Updates the state of the temp ent beams
	virtual void		UpdateTemeBeams() = 0;

	virtual void		DrawBeam(Beam_t* pbeam) = 0;
	virtual void		DrawBeam(C_Beam* pbeam, ITraceFilter* pEntityBeamTraceFilter = NULL) = 0;

	virtual	void		KillDeadBeams(IClientEntity* pDeadEntity) = 0;

	virtual	void		CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1) = 0;
	virtual Beam_t* CreateBeamEnts(BeamInfo_t& beamInfo) = 0;

	virtual	void		CreateBeamEntPoint(int	nStartEntity, const Vector* pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual Beam_t* CreateBeamEntPoint(BeamInfo_t& beamInfo) = 0;

	virtual	void		CreateBeamPoints(Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual	Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo) = 0;

	virtual	void		CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags) = 0;
	virtual Beam_t* CreateBeamRing(BeamInfo_t& beamInfo) = 0;

	virtual void		CreateBeamRingPoint(const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags) = 0;
	virtual Beam_t* CreateBeamRingPoint(BeamInfo_t& beamInfo) = 0;

	virtual	void		CreateBeamCirclePoints(int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float endWidth, float fadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b) = 0;
	virtual Beam_t* CreateBeamCirclePoints(BeamInfo_t& beamInfo) = 0;

	virtual	void		CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float endWidth, float fadeLength, float r, float g, float b,
		float brightness) = 0;
	virtual Beam_t* CreateBeamFollow(BeamInfo_t& beamInfo) = 0;
};