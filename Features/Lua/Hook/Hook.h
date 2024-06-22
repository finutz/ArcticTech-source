#pragma once

#include "../Sol.hpp"

#include <map>

struct LuaHook_t {
	int scriptId;
	sol::protected_function func;
};

enum ELuaCallbacks {
	LUA_RENDER,
	LUA_CREATEMOVE,
	LUA_ANTIAIM,
	LUA_FRAMESTAGE,
	LUA_LEVELINIT,
	LUA_GAMEEVENTS,
	LUA_AIM_SHOT,
	LUA_AIM_ACK,
	LUA_VOICE_DATA,
	LUA_PRE_ANIMUPDATE,
	LUA_POST_ANIMUPDATE,
	LUA_LOCAL_ALPHA,
	LUA_DRAW_CHAMS,
	LUA_UNLOAD,
	LUA_MAX_CALLBACKS
};

class CLuaHookManager {
public:
	void registerHook(ELuaCallbacks eventName, int scriptId, sol::protected_function func);
	void unregisterHooks(int scriptId);
	std::vector<LuaHook_t> getHooks(ELuaCallbacks callback);
	void removeAll();

private:
	std::vector<LuaHook_t> hooks[LUA_MAX_CALLBACKS];
};