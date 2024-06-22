#include "CGlobalVarsBase.h"

#include "../Globals.h"

int INetChannel::SendDatagram() {
    using Fn = int(__thiscall*)(void*, void*);
    return CallVFunction<Fn>(this, 46)(this, 0);
}

bool INetChannel::SendNetMsg(void* msg, bool bForceReliable, bool bVoice) {
    using Fn = bool(__thiscall*)(void*, void*, bool, bool);
    return CallVFunction<Fn>(this, 40)(this, msg, bForceReliable, bVoice);
}