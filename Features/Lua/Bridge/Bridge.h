#pragma once

#include <filesystem>
#include <string>
#include <vector>

#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "luajit.lib")


#define SOL_SAFE_FUNCTION_CALLS 1
#define SOL_SAFE_FUNCTION 1
#include "../Hook/Hook.h"
#include "../../../Utils/Console.h"
#include "../../../SDK/Globals.h"

struct LuaScript_t {
	bool loaded = false;
	std::string name;
	std::string ui_name; // added star if loaded
	std::filesystem::path path;
	sol::environment* env = nullptr;
	std::vector<CMenuTab*> tabs;
	std::vector<CMenuGroupbox*> groupboxes;
	std::vector<IBaseWidget*> ui_elements;
};

struct CUserCmd_lua {
	int command_number = 0;
	int tickcount = 0;
	Vector move;
	QAngle viewangles;
	int buttons = 0;
	int random_seed = false;
	sol::object override_defensive = sol::nil;
	bool allow_defensive = true;

	void set_move(float yaw, sol::optional<float> speed) {
		float sp = 0.f;
		if (speed.has_value()) {
			sp = speed.value();
		} else {
			sp = move.Q_Length();
		}

		move = Math::AngleVectors(QAngle(0.f, viewangles.yaw - yaw, 0.f)) * sp;
	}
};

template <typename T>
struct ArrayWrapper_Lua {
	T* layers;

	ArrayWrapper_Lua(T* arr) : layers(arr) {}

	T& get(int index) {
		return layers[index];
	}

	void set(int index, const T& val) {
		layers[index] = val;
	}
};

class CLua
{
public:
	int GetScriptID( std::string name );
	std::string GetScriptPath( std::string name );
	std::string GetScriptPath( int id );
	std::vector<std::string> GetUIList();

	void LoadScript( int id );
	void UnloadScript( int id );
	void UnloadAll();
	void ReloadAll();
	std::vector<std::string> GetLoadedScripts();

	void RefreshScripts();
	void RefreshUI();

	void Setup();

	CLuaHookManager hooks;
	std::vector<LuaScript_t> scripts;
};

extern CLua* Lua;