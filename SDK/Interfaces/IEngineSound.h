#pragma once
#include "../Misc/UtlVector.h"

struct Vector;

enum soundlevel_t
{
	SNDLVL_NONE = 0,
	SNDLVL_20dB = 20,		// rustling leaves
	SNDLVL_25dB = 25,		// whispering
	SNDLVL_30dB = 30,		// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,		// refrigerator
	SNDLVL_50dB = 50,	    // 3.9    // average home
	SNDLVL_55dB = 55,	    // 3.0
	SNDLVL_IDLE = 60,	    // 2.0	
	SNDLVL_60dB = 60,	    // 2.0	// normal conversation, clothes dryer
	SNDLVL_65dB = 65,	    // 1.5	// washing machine, dishwasher
	SNDLVL_STATIC = 66,	    // 1.25
	SNDLVL_70dB = 70,	    // 1.0	// car, vacuum cleaner, mixer, electric sewing machine
	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	    // 0.8	// busy traffic
	SNDLVL_80dB = 80,	    // 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80,    // 0.7
	SNDLVL_85dB = 85,	    // 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	    // 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	    // 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,     // helicopter, power mower
	SNDLVL_110dB = 110,     // snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,     // auto horn, propeller aircraft
	SNDLVL_130dB = 130,     // air raid siren
	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	    // 0.2
	SNDLVL_150dB = 150,	    // 0.2
	SNDLVL_180dB = 180,		// rocket launching
};

typedef unsigned int HSOUNDSCRIPTHASH;
typedef void* FileNameHandle_t;
#define SOUNDEMITTER_INVALID_HASH	(HSOUNDSCRIPTHASH)-1

struct SndInfo_t
{
	int			m_nGuid;
	FileNameHandle_t m_filenameHandle;
	int			m_nSoundSource;
	int			m_nChannel;
	int			m_nSpeakerEntity;
	float		m_flVolume;	
	float		m_flLastSpatializedVolume;
	float		m_flRadius;
	int			m_nPitch;
	Vector*		m_pOrigin;
	Vector*		m_pDirection;
	bool		m_bUpdatePositions;
	bool		m_bIsSentence;
	bool		m_bDryMix;
	bool		m_bSpeaker;
	bool		m_bFromServer;
};


struct SoundInfo_t
{
	Vector			vOrigin;
	Vector			vDirection;
	Vector			vListenerOrigin;
	const char* pszName;		// UNDONE: Make this a FilenameHandle_t to avoid bugs with arrays of these
	float			fVolume;
	float			fDelay;
	float			fTickTime;			// delay is encoded relative to this tick, fix up if packet is delayed
	int				nSequenceNumber;
	int				nEntityIndex;
	int				nChannel;
	int				nPitch;
	int				nFlags;
	unsigned int 	nSoundNum;
	int				nSpeakerEntity;
	int				nRandomSeed;
	soundlevel_t	Soundlevel;
	bool			bIsSentence;
	bool			bIsAmbient;
	bool			bLooping;
};

#define	PITCH_NORM		100			  // non-pitch shifted
#define PITCH_LOW		95			    // other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH		120

class CSfxTable;
struct StartSoundParams_t
{
	bool			bStaticSound;
	int				iUserData;
	int				iSoundSource;
	int				iEntityChannel;
	CSfxTable* pSfx;
	Vector			vecOrigin;
	Vector			vecDirection;
	bool			bUpdatePositions;
	float			flVolume;
	ESoundLevel		soundLevel;
	int				iFlags;
	int				iPitch;
	bool			bFromServer;
	float			flDelay;
	int				nSpeakerEntity;
	bool			bSuppressRecording;
	int				iInitialStreamPosition;
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() { }

	virtual bool	IsReliable() const = 0;
	virtual bool	IsInitMessage() const = 0;

	virtual int		GetRecipientCount() const = 0;
	virtual int		GetRecipientIndex(int nSlot) const = 0;
};

class IEngineSound
{
public:
	virtual bool	PrecacheSound(const char* szSample, bool bPreload = false, bool bIsUISound = false) = 0;
	virtual bool	IsSoundPrecached(const char* szSample) = 0;
	virtual void	PrefetchSound(const char* szSample) = 0;
	virtual bool	IsLoopingSound(const char* szSample) = 0;
	virtual float	GetSoundDuration(const char* szSample) = 0;

	virtual int EmitSound(IRecipientFilter& filter, int nEntityIndex, int iChannel, const char* szSoundEntry, unsigned int uSoundEntryHash, const char* szSample,
		float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
		const Vector* vecOrigin, const Vector* vecDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float flSoundTime, int nSpeakerEntity, StartSoundParams_t& parameters) = 0;

	virtual int EmitSound(IRecipientFilter& filter, int nEntityIndex, int iChannel, const char* szSoundEntry, unsigned int uSoundEntryHash, const char* szSample,
		float flVolume, ESoundLevel nSoundLevel, int nSeed, int iFlags, int iPitch,
		const Vector* vecOrigin, const Vector* vecDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float flSoundTime, int nSpeakerEntity, StartSoundParams_t& parameters) = 0;

	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int nEntityIndex, int iChannel, int nSentenceIndex,
		float flVolume, ESoundLevel nSoundLevel, int nSeed, int iFlags = 0, int iPitch = PITCH_NORM,
		const Vector* vecOrigin = nullptr, const Vector* vecDirection = nullptr, CUtlVector<Vector>* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float flSoundTime = 0.0f, int nSpeakerEntity = -1) = 0;

	virtual void	StopSound(int nEntityIndex, int iChannel, const char* szSample, unsigned int uSoundEntryHash) = 0;
	virtual void	StopAllSounds(bool bClearBuffers) = 0;
	virtual void	SetRoomType(IRecipientFilter& filter, int iRoomType) = 0;
	virtual void	SetPlayerDSP(IRecipientFilter& filter, int iDspType, bool bFastReset) = 0;
	virtual void	EmitAmbientSound(const char* szSample, float flVolume, int iPitch = PITCH_NORM, int iFlags = 0, float flSoundTime = 0.0f) = 0;
	virtual float	GetDistGainFromSoundLevel(ESoundLevel soundLevel, float flDistance) = 0;
	virtual int		GetGuidForLastSoundEmitted() = 0;
	virtual bool	IsSoundStillPlaying(int nGUID) = 0;
	virtual void	StopSoundByGuid(int nGUID, bool bForceSync) = 0;
	virtual void	SetVolumeByGuid(int nGUID, float flVolume) = 0;
private:
	virtual void	function0() = 0;
public:
	virtual void	GetActiveSounds(CUtlVector<SndInfo_t>& vecSoundList) = 0;
	virtual void	PrecacheSentenceGroup(const char* szGroupName) = 0;
	virtual void	NotifyBeginMoviePlayback() = 0;
	virtual void	NotifyEndMoviePlayback() = 0;
	virtual bool	GetSoundChannelVolume(const char* szSound, float& flVolumeLeft, float& flVolumeRight) = 0;
	virtual float	GetElapsedTimeByGuid(int nGUID) = 0;
	virtual bool	GetPreventSound() = 0;
	virtual void	SetReplaySoundFade(float flReplayVolume) = 0;
	virtual float	GetReplaySoundFade() const = 0;
};