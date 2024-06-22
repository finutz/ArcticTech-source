#include "Hook.h"

void CLuaHookManager::registerHook(ELuaCallbacks event, int scriptId, sol::protected_function func) {
	LuaHook_t hk = { scriptId, func };

	hooks[event].push_back(hk); // return hook function
}

void CLuaHookManager::unregisterHooks(int scriptId) {
	for (int i = 0; i < LUA_MAX_CALLBACKS; i++) {
		auto& cbs = hooks[i];

		for (auto it = cbs.begin(); it != cbs.end();) {
			if (it->scriptId == scriptId) {
				it = cbs.erase(it);
				continue;
			}

			it++;
		}
	}
}

std::vector<LuaHook_t> CLuaHookManager::getHooks(ELuaCallbacks eventName) {
	return hooks[eventName];
}

void CLuaHookManager::removeAll() {
	for (int i = 0; i < LUA_MAX_CALLBACKS; i++) {
		auto& cbs = hooks[i];

		cbs.clear();
	}
}