#pragma once
#include <cstdint>
#include "../Misc/QAngle.h"
#include "../../Utils/VitualFunction.h"

class INetMessage;

class CGlobalVarsBase
{
public:
    float     realtime;                     // 0x0000
    int       framecount;                   // 0x0004
    float     absoluteframetime;            // 0x0008
    float     absoluteframestarttimestddev; // 0x000C
    float     curtime;                      // 0x0010
    float     frametime;                    // 0x0014
    int       max_clients;                   // 0x0018
    int       tickcount;                    // 0x001C
    float     interval_per_tick;            // 0x0020
    float     interpolation_amount;         // 0x0024
    int       simTicksThisFrame;            // 0x0028
    int       network_protocol;             // 0x002C
    void* pSaveData;                    // 0x0030
    bool      m_bClient;                    // 0x0031
    bool      m_bRemoteClient;              // 0x0032

    inline void store();
    inline void restore();
private:
    // 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
    int       nTimestampNetworkingBase;
    // 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to Set the networking basis, prevents
    //  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
    int       nTimestampRandomizeWindow;

};

inline CGlobalVarsBase s_RestoreGlobalsBase;

inline void CGlobalVarsBase::store() {
    memcpy(&s_RestoreGlobalsBase, this, sizeof(CGlobalVarsBase));
}

inline void CGlobalVarsBase::restore() {
    memcpy(this, &s_RestoreGlobalsBase, sizeof(CGlobalVarsBase));
}

class CClockDriftMgr
{
public:
    float m_ClockOffsets[16];   //0x0000
    uint32_t m_iCurClockOffset; //0x0044
    uint32_t m_nServerTick;     //0x0048
    uint32_t m_nClientTick;     //0x004C
};

class INetChannel
{
public:
    byte	pad0[0x14];				//0x0000
    bool		m_bProcessingMessages;	//0x0014
    bool		m_bShouldDelete;			//0x0015
    bool		m_bStopProcessing;		//0x0016
    byte	pad1[0x1];				//0x0017
    int			m_nOutSequenceNr;			//0x0018 last send outgoing sequence number
    int			m_nInSequenceNr;			//0x001C last received incoming sequence number
    int			m_nOutSequenceNrAck;		//0x0020 last received acknowledge outgoing sequence number
    int			m_iOutReliableState;		//0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
    int			m_iInReliableState;		//0x0028 state of incoming reliable data
    int			m_nChokedPackets;			//0x002C number of choked packets
    byte	pad2[0x414];			//0x0030

    int SendDatagram();
    bool SendNetMsg(void* msg, bool bForceReliable, bool bVoice);
};

class InterfaceReg
{
private:
    using InstantiateInterfaceFn = void* ( * )( );
public:
    InstantiateInterfaceFn m_CreateFn;
    const char* m_pName;
    InterfaceReg* m_pNext;
};

class CClientState
{
public:
    void ForceFullUpdate()
    {
        m_nDeltaTick = -1;
    }

    char pad_0000[156];
    INetChannel* m_NetChannel;
    int m_nChallengeNr;
    char pad_00A4[100];
    int m_nSignonState;
    int signon_pads[2];
    float m_flNextCmdTime;
    int m_nServerCount;
    int m_nCurrentSequence;
    int musor_pads[2];
    CClockDriftMgr m_ClockDriftMgr;
    int m_nDeltaTick;
    bool m_bPaused;
    char paused_align[3];
    int m_nViewEntity;
    int m_nPlayerSlot;
    int bruh;
    char m_szLevelName[260];
    char m_szLevelNameShort[80];
    char m_szGroupName[80];
    char pad_032[92];
    int m_nMaxClients;
    char pad_0314[18828];
    float m_nLastServerTickTime;
    bool m_bInSimulation;
    char pad_4C9D[3];
    int m_nOldTickCount;
    float m_flTickReminder;
    float m_flFrametime;
    int m_nLastOutgoingCommand;
    int m_nChokedCommands;
    int m_nLastCommandAck;
    int m_nPacketEndTickUpdate;
    int m_nCommandAck;
    int m_nSoundSequence;
    char pad_4CCD[76];
    QAngle viewangles;

    int GetChokedCommands() {
        return *(int*)((uintptr_t)this + 0x4D30);
    }
};