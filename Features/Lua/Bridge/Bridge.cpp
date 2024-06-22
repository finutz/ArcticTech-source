#include "Bridge.h"

#include "../../ShotManager/ShotManager.h"
#include "../../RageBot/LagCompensation.h"
#include "../../../SDK/Requests.h"
#include "../../../SDK/NetMessages.h"
#include "../../Visuals/ESP.h"
#include "../../AntiAim/AntiAim.h"
#include "../../RageBot/Exploits.h"
#include "../../RageBot/AutoWall.h"
#include "../../Visuals/WeaponIcons.h"
#include "../../../SDK/Misc/KeyValues.h"
#include "../../Visuals/Chams.h"

#define DEF_LUA_ARR(type) lua.new_usertype<ArrayWrapper_Lua<type>>((std::string)#type + "_array", sol::no_constructor, "__index", &ArrayWrapper_Lua<type>::get, "__newindex", &ArrayWrapper_Lua<type>::set)

std::vector<UILuaCallback_t> g_ui_lua_callbacks;

void LuaSaveConfig(LuaScript_t* script) {
	nlohmann::json result;

	for (auto e : script->ui_elements) {\
		std::string name = e->parent->name + "_" + e->name + "_" + std::to_string(static_cast<int>(e->GetType()));

		switch (e->GetType()) {
		case WidgetType::Checkbox:
			result[name] = ((CCheckBox*)e)->value;
			break;
		case WidgetType::ColorPicker:
			result[name] = ((CColorPicker*)e)->value;
			break;
		case WidgetType::KeyBind:
			result[name] = { ((CKeyBind*)e)->mode, ((CKeyBind*)e)->key };
			break;
		case WidgetType::SliderInt:
			result[name] = ((CSliderInt*)e)->value;
			break;
		case WidgetType::SliderFloat:
			result[name] = ((CSliderFloat*)e)->value;
			break;
		case WidgetType::Combo:
			result[name] = ((CComboBox*)e)->value;
			break;
		case WidgetType::MultiCombo: {
			int temp = 0;
			for (int i = 0; i < ((CMultiCombo*)e)->elements.size(); i++)
				if (((CMultiCombo*)e)->value[i])
					temp |= 1 << i;
			result[name] = temp;
			break;
		}
		case WidgetType::Input:
			result[name] = std::string(((CInputBox*)e)->buf);
			break;
		default:
			// handle any unexpected values of WidgetType here
			break;
		}
	}

	const std::string file_path = std::filesystem::current_path().string() + "/at/scripts/cfg/" + script->name + ".cfg";

	std::ofstream file(file_path);

	file << result;
}

void LuaLoadConfig(LuaScript_t* script) {
	const std::string file_path = std::filesystem::current_path().string() + "/at/scripts/cfg/" + script->name + ".cfg";
	if (!std::filesystem::exists(file_path)) {
		// not saved config for lua yet
		return;
	}

	std::ifstream file(file_path);
	nlohmann::json config_json;
	file >> config_json;

	for (auto e : script->ui_elements) {
		try {
			auto& val = config_json[e->parent->name + "_" + e->name + "_" + std::to_string(static_cast<int>(e->GetType()))];

			switch (e->GetType()) {
			case WidgetType::Checkbox:
				((CCheckBox*)e)->value = val;
				break;
			case WidgetType::ColorPicker:
				((CColorPicker*)e)->value[0] = val[0];
				((CColorPicker*)e)->value[1] = val[1];
				((CColorPicker*)e)->value[2] = val[2];
				((CColorPicker*)e)->value[3] = val[3];
				break;
			case WidgetType::KeyBind:
				((CKeyBind*)e)->mode = val[0];
				((CKeyBind*)e)->key = val[1];
				break;
			case WidgetType::SliderInt:
				((CSliderInt*)e)->value = val;
				break;
			case WidgetType::SliderFloat:
				((CSliderFloat*)e)->value = val;
				break;
			case WidgetType::Combo:
				((CComboBox*)e)->value = val;
				break;
			case WidgetType::MultiCombo:
				for (int i = 0; i < ((CMultiCombo*)e)->elements.size(); i++)
					((CMultiCombo*)e)->value[i] = val & (1 << i);
				break;
			case WidgetType::Input: {
				ZeroMemory(((CInputBox*)e)->buf, 64);
				std::string inp = val;
				memcpy(((CInputBox*)e)->buf, inp.c_str(), inp.size());
				break;
			}
			default:
				break;
			}

			if (e->GetType() != WidgetType::Button) {
				for (auto cb : e->callbacks)
					cb();

				for (auto& lcb : e->lua_callbacks)
					lcb.func();
			}
		}
		catch (nlohmann::json::exception& ex) {
			Console->Error("could not find item: " + e->name);
		}
	}
}

nlohmann::json luaTableToJson(sol::table table) {
	nlohmann::json result;

	for (const auto& pair : table) {
		if (pair.second.is<sol::table>()) {
			if (pair.first.is<int>())
				result[pair.first.as<int>()] = luaTableToJson(pair.second.as<sol::table>());
			else
				result[pair.first.as<std::string>()] = luaTableToJson(pair.second.as<sol::table>());
		} else {
			if (pair.first.is<int>())
				result[pair.first.as<int>()] = pair.second.as<std::string>();
			else
				result[pair.first.as<std::string>()] = pair.second.as<std::string>();
		}
	}

	return result;
}


sol::state lua;

sol::table jsonToLuaTable(lua_State* state, nlohmann::json json);

sol::table jsonToLuaArray(lua_State* state, nlohmann::json json) {
	sol::table tmp = lua.create_table();
	int index = 1;
	for (const auto& element : json) {
		if (element.is_null()) {
			tmp[sol::create_if_nil][index++] = sol::nil;
		}
		else if (element.is_boolean()) {
			tmp[sol::create_if_nil][index++] = element.get<bool>();
		}
		else if (element.is_number_float()) {
			tmp[sol::create_if_nil][index++] = element.get<float>();
		}
		else if (element.is_number_integer()) {
			tmp[sol::create_if_nil][index++] = element.get<int>();
		}
		else if (element.is_string()) {
			tmp[sol::create_if_nil][index++] = element.get<std::string>();
		}
		else if (element.is_object()) {
			tmp[sol::create_if_nil][index++] = jsonToLuaTable(state, element);
		}
		else if (element.is_array()) {
			tmp[sol::create_if_nil][index++] = jsonToLuaArray(state, element);
		}
	}

	return tmp;
}

sol::table jsonToLuaTable(lua_State* state, nlohmann::json json) {
	sol::table result = lua.create_table(state);

	if (json.is_array())
		return jsonToLuaArray(state, json);

	for (auto& pair : json.items()) {
		auto& key = pair.key();
		auto& value = pair.value();

		if (value.is_object()) {
			result[sol::create_if_nil][pair.key()] = jsonToLuaTable(state, value.get<nlohmann::json>());
		}
		else if (value.is_array()) {
			result[sol::create_if_nil][pair.key()] = jsonToLuaArray(state, value);
		}
		else if (value.is_null()) {
			result[sol::create_if_nil][key] = sol::nil;
		}
		else if (value.is_boolean()) {
			result[sol::create_if_nil][key] = value.get<bool>();
		}
		else if (value.is_number()) {
			result[sol::create_if_nil][key] = value.get<double>();
		}
		else if (value.is_string()) {
			result[sol::create_if_nil][key] = value.get<std::string>();
		}
	}

	return result;
}

std::string GetCurrentScript(sol::this_state s) {
	sol::state_view lua_state(s);
	sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
	std::string source = rs["source"];
	std::string filename = std::filesystem::path(source.substr(1)).stem().string();

	return filename;
}

void LuaErrorHandler(sol::optional<std::string> message) {
	if (!message)
		return;

	Console->ArcticTag();
	Console->ColorPrint(message.value_or("unknown"), Color(255, 0, 0));
	Console->Print("\n");
}

void ScriptLoadButton()
{
	Lua->LoadScript(Lua->GetScriptID(Config->lua_list->get_name()));
}

void ScriptUnloadButton()
{
	Lua->UnloadScript(Lua->GetScriptID(Config->lua_list->get_name()));
}

void ScriptSaveButton() {
	for (auto& script : Lua->scripts) {
		if (script.loaded)
			LuaSaveConfig(&script);
	}
}

namespace api {
	void print(sol::this_state staet, sol::object msg) {
		Console->Log(lua["tostring"](msg));
	}

	void error(std::string msg) {
		Console->Error(msg);
	}

	void print_raw(std::string msg, sol::optional<Color> color) {
		Console->ColorPrint(msg, color.value_or(Color(255, 255, 255)));
	}

	void safe_call(sol::protected_function func) {
		auto result = func();

		if (!result.valid()) {
			sol::error er = result;
			Console->Error(er.what());
		}
	}

	namespace client {
		void add_callback(sol::this_state state, std::string event_name, sol::protected_function func) {
			const std::string script_name = GetCurrentScript(state);
			const int script_id = Lua->GetScriptID(script_name);

			if (event_name == "render")
				Lua->hooks.registerHook(LUA_RENDER, script_id, func);
			else if (event_name == "createmove")
				Lua->hooks.registerHook(LUA_CREATEMOVE, script_id, func);
			else if (event_name == "antiaim")
				Lua->hooks.registerHook(LUA_ANTIAIM, script_id, func);
			else if (event_name == "level_init")
				Lua->hooks.registerHook(LUA_LEVELINIT, script_id, func);
			else if (event_name == "aim_shot")
				Lua->hooks.registerHook(LUA_AIM_SHOT, script_id, func);
			else if (event_name == "aim_ack")
				Lua->hooks.registerHook(LUA_AIM_ACK, script_id, func);
			else if (event_name == "frame_stage")
				Lua->hooks.registerHook(LUA_FRAMESTAGE, script_id, func);
			else if (event_name == "game_events")
				Lua->hooks.registerHook(LUA_GAMEEVENTS, script_id, func);
			else if (event_name == "unload")
				Lua->hooks.registerHook(LUA_UNLOAD, script_id, func);
			else if (event_name == "voice_message")
				Lua->hooks.registerHook(LUA_VOICE_DATA, script_id, func);
			else if (event_name == "pre_anim_update")
				Lua->hooks.registerHook(LUA_PRE_ANIMUPDATE, script_id, func);
			else if (event_name == "post_anim_update")
				Lua->hooks.registerHook(LUA_POST_ANIMUPDATE, script_id, func);
			else if (event_name == "local_alpha")
				Lua->hooks.registerHook(LUA_LOCAL_ALPHA, script_id, func);
			else if (event_name == "draw_chams")
				Lua->hooks.registerHook(LUA_DRAW_CHAMS, script_id, func);
			else
				Console->Error(std::format("[{}] unknown callback: {}", GetCurrentScript(state), event_name));
		}

		sol::object event_index(sol::this_state state, IGameEvent* event, std::string key) {
			KeyValues* val_k = (*reinterpret_cast<KeyValues**>(reinterpret_cast<uintptr_t>(event) + 0x8))->FindKey(key.c_str());

			if (!val_k)
				return sol::nil;

			switch (val_k->GetDataType())
			{
			case KeyValues::TYPE_STRING:
				return sol::make_object(state, val_k->m_sValue);
			case KeyValues::TYPE_INT:
				return sol::make_object(state, val_k->m_iValue);
			case KeyValues::TYPE_FLOAT:
				return sol::make_object(state, val_k->m_flValue);
			case KeyValues::TYPE_PTR:
				return sol::make_object(state, val_k->m_pValue);
			case KeyValues::TYPE_WSTRING:
				return sol::make_object(state, val_k->m_wsValue);
			case KeyValues::TYPE_COLOR:
				return sol::make_object(state, Color(val_k->m_Color[0], val_k->m_Color[1], val_k->m_Color[2], val_k->m_Color[3]));
			case KeyValues::TYPE_UINT64:
				return sol::make_object(state, val_k->m_iValue);
			default:
				return sol::make_object(state, val_k->m_pValue);
			}
		}

		void unload_script(sol::this_state state) {
			Lua->UnloadScript(Lua->GetScriptID(GetCurrentScript(state)));
		}

		void reload_script(sol::this_state state) {
			Lua->UnloadScript(Lua->GetScriptID(GetCurrentScript(state)));
			Lua->LoadScript(Lua->GetScriptID(GetCurrentScript(state)));
		}
	}

	namespace globals {
		bool is_connected() {
			return EngineClient->IsConnected();
		}

		bool is_in_game() {
			return EngineClient->IsInGame();
		}

		int choked_commands() {
			return ClientState->m_nChokedCommands;
		}

		int commandack() {
			return ClientState->m_nCommandAck;
		}

		int commandack_prev() {
			return ClientState->m_nLastCommandAck;
		}

		int last_outgoing_command() {
			return ClientState->m_nLastOutgoingCommand;
		}

		int server_tick() {
			return ClientState->m_ClockDriftMgr.m_nServerTick;
		}

		int client_tick() {
			return ClientState->m_ClockDriftMgr.m_nClientTick;
		}

		int delta_tick() {
			return ClientState->m_nDeltaTick;
		}

		int clock_offset() {
			return ClientState->m_ClockDriftMgr.m_iCurClockOffset;
		}
	}

	namespace common {
		std::string get_map_shortname() {
			return EngineClient->GetLevelNameShort();
		}

		std::string get_map_name() {
			return EngineClient->GetLevelName();
		}

		bool is_recording_voice() {
			return EngineClient->IsVoiceRecording();
		}
	}

	namespace files {
		std::string read(std::string path) {
			if (!std::filesystem::exists(path)) {
				Console->Error(std::format("files.read: file {} doesn't exists!", path));
				return "";
			}

			std::ifstream file(path);
			std::stringstream buffer;
			buffer << file.rdbuf();

			return buffer.str();
		}

		void write(std::string path, std::string data) {
			std::ofstream file(path, std::ofstream::binary);

			file.clear();
			file.write(data.c_str(), data.size());
			file.close();
		}

		void create_folder(std::string path) {
			std::filesystem::create_directory(path);
		}

		bool exists(std::string path) {
			return std::filesystem::exists(path);
		}
	}

	namespace json {
		std::string stringify(sol::table table) {
			nlohmann::json json = luaTableToJson(table);

			return json.dump();
		}

		sol::table parse(sol::this_state state, std::string data) {
			nlohmann::json json = nlohmann::json::parse(data);

			return jsonToLuaTable(state, json);
		}
	} 

	namespace vector {
		Vector closes_ray_point(Vector self, Vector start, Vector end) {
			return EngineTrace->ClosestPoint(start, end, self);
		}

		float dist_to_ray(Vector self, Vector start, Vector end) {
			return EngineTrace->DistanceToRay(start, end, self);
		}

		Vector lerp(Vector self, Vector other, float weight) {
			return self + (other - self) * weight;
		}

		Vector to_screen(Vector self) {
			Vector2 scr = Render->WorldToScreen(self);
			return Vector(scr.x, scr.y);
		}
		
		std::string tostring(Vector self) {
			return std::format("vector({}, {}, {})", self.x, self.y, self.z);
		}

		sol::object angles(sol::this_state state, Vector self, sol::optional<QAngle> angle) {
			if (angle.has_value())
				return sol::make_object(state, Math::AngleVectors(angle.value()));
			return sol::make_object(state, Math::VectorAngles(self));
		}
	}

	namespace render {
		Vector screen_size() {
			return Vector(Cheat.ScreenSize.x, Cheat.ScreenSize.y);
		}

		QAngle camera_angles(sol::optional<QAngle> angle) {
			QAngle result;
			EngineClient->GetViewAngles(&result);

			if (angle.has_value()) {
				EngineClient->SetViewAngles(&angle.value());
			}

			return result;
		}

		DXImage get_weapon_icon(int weap_id) {
			return WeaponIcons->GetIcon(weap_id);
		}

		void add_font(std::string mem) {
			Render->AddFontFromMemory(mem.data(), mem.size());
		}

		D3DXFont* load_font(std::string name, int size, std::string flags) {
			return Render->LoadFont(name, size, (flags.find("b") != std::string::npos) ? 600 : 400, (flags.find("a") != std::string::npos) ? CLEARTYPE_NATURAL_QUALITY : 0);
		}

		DXImage load_image(std::string mem, Vector size) {
			return Render->LoadImageFromMemory(mem.data(), mem.size(), size.to_vec2());
		}

		void blur(Vector start, Vector end, sol::optional<float> borderRadius, sol::optional<float> blurWeight, sol::optional<Color> blurColor) {
			Render->Blur(start.to_vec2(), end.to_vec2(), borderRadius.value_or(0.f), blurWeight.value_or(8.f), blurColor.value_or(Color()));
		}

		void line(Vector start, Vector end, Color col) {
			Render->Line(start.to_vec2(), end.to_vec2(), col);
		}

		void poly(Color col, sol::variadic_args vertecies) {
			std::vector<Vector2> verts;
			for (auto v : vertecies) {
				Vector vec = v;
				verts.push_back(Vector2(vec.x, vec.y));
			}
			Render->PolyFilled(verts, col);
		}

		void poly_line(Color col, sol::variadic_args vertecies) {
			std::vector<Vector2> verts;
			for (auto v : vertecies) {
				Vector vec = v;
				verts.push_back(Vector2(vec.x, vec.y));
			}
			Render->PolyLine(verts, col);
		}

		void rect(Vector start, Vector end, Color clr, sol::optional<int> rounding) {
			Render->BoxFilled(start.to_vec2(), end.to_vec2(), clr, rounding.value_or(0));
		}

		void rect_outline(Vector start, Vector end, Color clr, sol::optional<int> rounding) {
			Render->Box(start.to_vec2(), end.to_vec2(), clr, rounding.value_or(0));
		}

		void gradient(Vector start, Vector end, Color top_left, Color top_right, Color bottom_left, Color bottom_right) {
			Render->GradientBox(start.to_vec2(), end.to_vec2(), top_left, top_right, bottom_left, bottom_right);
		}

		void circle(Vector center, Color clr, float radius, sol::optional<float> start_deg, sol::optional<float> pct) {
			Render->CircleFilled(center.to_vec2(), radius, clr);
		}

		void circle_outline(Vector center, Color clr, float radius, sol::optional<float> start_deg, sol::optional<float> pct, sol::optional<int> thickness) {
			Render->Circle(center.to_vec2(), radius, clr, -1, start_deg.value_or(0.f), start_deg.value_or(0.f) + pct.value_or(1.f) * 360.f);
		}

		void circle_gradient(Vector center, Color color_outer, Color color_inner, float radius) {
			Render->GlowCircle2(center.to_vec2(), radius, color_inner, color_outer);
		}

		void circle_3d(Vector center, Color color, float radius) {
			Render->Circle3D(center, radius, color, true);
		}

		void circle_3d_outline(Vector center, Color color, float radius) {
			Render->Circle3D(center, radius, color, true);
		}

		void texture(DXImage texture, Vector pos, std::optional<Color> col) {
			Render->Image(texture, pos.to_vec2(), col.value_or(Color(255, 255, 255)));
		}

		void text(sol::this_state state, sol::object font, Vector position, Color color, std::string flags, std::string text) {
			int flags_ = 0;
			for (char c : flags) {
				if (c == 'c')
					flags_ |= TEXT_CENTERED;
				else if (c == 'o')
					flags_ |= TEXT_OUTLINED;
				else if (c == 'd')
					flags_ |= TEXT_DROPSHADOW;
			}

			D3DXFont* font_ = nullptr;

			if (font.is<int>()) {
				switch (font.as<int>()) {
				case 1:
					font_ = Verdana;
					break;
				case 2:
					font_ = SmallFont;
					break;
				case 3:
					font_ = VerdanaBold;
					break;
				case 4:
					font_ = CalibriBold;
					break;
				default:
					Console->Error(std::format("[{}] unknown font: {}", GetCurrentScript(state), font.as<int>()));
					return;
				}
			}
			else {
				font_ = font.as<D3DXFont*>();

				if (!font_) {
					Console->Error(std::format("[{}] passed nil font", GetCurrentScript(state)));
					return;
				}
			}

			Render->Text(text, position.to_vec2(), color, font_, flags_);
		}

		Vector measure_text(sol::this_state state, sol::object font, std::string text) {
			D3DXFont* font_ = nullptr;

			if (font.is<int>()) {
				switch (font.as<int>()) {
				case 1:
					font_ = Verdana;
					break;
				case 2:
					font_ = SmallFont;
					break;
				case 3:
					font_ = VerdanaBold;
					break;
				case 4:
					font_ = CalibriBold;
					break;
				default:
					Console->Error(std::format("[{}] unknown font: {}", GetCurrentScript(state), font.as<int>()));
					return Vector();
				}
			}
			else {
				font_ = font.as<D3DXFont*>();

				if (!font_) {
					Console->Error(std::format("[{}] passed nil font", GetCurrentScript(state)));
					return Vector();
				}
			}

			Vector2 res = Render->CalcTextSize(text, font_);
			return Vector(res.x, res.y);
		}

		void vertex(int draw_type, sol::variadic_args vertecies) {
			std::vector<Vertex> verts;

			for (auto x : vertecies)
				verts.push_back(x);

			int prim_count = verts.size();

			switch (draw_type) {
			case D3DPT_LINELIST:
				prim_count = verts.size() / 2;
				break;
			case D3DPT_LINESTRIP:
				prim_count = verts.size() - 1;
				break;
			case D3DPT_TRIANGLELIST:
				prim_count = verts.size() / 3;
				break;
			case D3DPT_TRIANGLESTRIP:
			case D3DPT_TRIANGLEFAN:
				prim_count = verts.size() - 2;
				break;
			}

			Render->Vertecies(draw_type, prim_count, verts);
		}

		void push_clip_rect(Vector start, Vector end) {
			Render->PushClipRect(start.to_vec2(), end.to_vec2());
		}

		void pop_clip_rect() {
			Render->PopClipRect();
		}

		void set_antialias(bool state) {
			Render->SetAntiAliasing(state);
		}
	}

	namespace utils {
		void console_exec(std::string command) {
			EngineClient->ExecuteClientCmd(command.c_str());
		}

		void random_seed(int seed) {
			Utils::RandomSeed(seed);
		}

		int random_int(int min, int max) {
			return Utils::RandomInt(min, max);
		}

		float random_float(float min, float max) {
			return Utils::RandomFloat(min, max);
		}

		uintptr_t create_interface(std::string module_name, std::string interface_name) {
			return reinterpret_cast<uintptr_t>(Utils::CreateInterface(module_name.c_str(), interface_name.c_str()));
		}

		uintptr_t pattern_scan(std::string module_name, std::string pattern) {
			return reinterpret_cast<uintptr_t>(Utils::PatternScan(module_name.c_str(), pattern.c_str()));
		}

		CGameTrace trace_line(Vector start, Vector end, int mask, CBaseEntity* skip_entity) {
			return EngineTrace->TraceRay(start, end, mask, skip_entity);
		}

		CGameTrace trace_hull(Vector start, Vector end, Vector mins, Vector maxs, int mask, CBaseEntity* skip) {
			return EngineTrace->TraceHull(start, end, mins, maxs, mask, skip);
		}

		FireBulletData_t trace_bullet(CBasePlayer* attacker, Vector start, Vector end, sol::optional<CBasePlayer*> target) {
			FireBulletData_t data;
			AutoWall->FireBullet(attacker, start, end, data, target.value_or(nullptr));
			return data;
		}

		bool is_key_pressed(int key) {
			return GetAsyncKeyState(key) & 0x8000;
		}

		Vector get_mouse_position() {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetForegroundWindow(), &pt);

			return Vector(pt.x, pt.y);
		}

		void send_voice_message(SharedVoiceData_t voice_data) {
			NetMessages->SendNetMessage(&voice_data);
		}
	}

	namespace ui {
		sol::object find(sol::this_state state, std::string tab, std::string group, sol::optional<std::string> widget, sol::optional<WidgetType> wtype) {
			if (!widget.has_value()) {
				CMenuGroupbox* found_item = Menu->FindGroupbox(tab, group);

				if (!found_item) {
					Console->Error(std::format("[{}] cont find item: ({}, {})", GetCurrentScript(state), tab, group));
					return nullptr;
				}

				return sol::make_object(state, found_item);
			}

			WidgetType etype = wtype.value_or(WidgetType::Any);

			IBaseWidget* found_item = Menu->FindItem(tab, group, widget.value(), etype);

			if (!found_item) {
				Console->Error(std::format("[{}] cont find item: ({}, {}, {})", GetCurrentScript(state), tab, group, widget.value()));
				return nullptr;
			}

			return sol::make_object(state, found_item);
		}

		std::string element_get_name(sol::this_state state, IBaseWidget* widget) {
			return widget->name;
		}

		int element_type(IBaseWidget* widget) {
			return (int)widget->GetType();
		}

		sol::object element_get(sol::this_state state, IBaseWidget* element, sol::optional<int> index) {
			switch (element->GetType()) {
			case WidgetType::Checkbox:
				return sol::make_object(state, static_cast<CCheckBox*>(element)->get());
			case WidgetType::ColorPicker:
				return sol::make_object(state, static_cast<CColorPicker*>(element)->get());
			case WidgetType::KeyBind:
				return sol::make_object(state, static_cast<CKeyBind*>(element)->get());
			case WidgetType::SliderInt:
				return sol::make_object(state, static_cast<CSliderInt*>(element)->get());
			case WidgetType::SliderFloat:
				return sol::make_object(state, static_cast<CSliderFloat*>(element)->get());
			case WidgetType::Combo:
				return sol::make_object(state, static_cast<CComboBox*>(element)->get());
			case WidgetType::MultiCombo:
				return sol::make_object(state, static_cast<CMultiCombo*>(element)->get(index.value()));
			case WidgetType::Input:
				return sol::make_object(state, static_cast<CInputBox*>(element)->get());
			}

			Console->Error("trying to get unknown element");
		}

		int element_get_mode(sol::this_state state, IBaseWidget* element) {
			if (element->GetType() != WidgetType::KeyBind) {
				Console->Error("this element is not keybind!");
			}

			return static_cast<CKeyBind*>(element)->mode;
		}

		int element_get_key(sol::this_state state, IBaseWidget* element) {
			if (element->GetType() != WidgetType::KeyBind) {
				Console->Error("this element is not keybind!");
			}

			return static_cast<CKeyBind*>(element)->key;
		}

		void element_set(sol::this_state state, IBaseWidget* element, sol::object val, sol::optional<int> index) {
			switch (element->GetType()) {
			case WidgetType::Checkbox:
				static_cast<CCheckBox*>(element)->value = val.as<bool>();
				break;
			case WidgetType::ColorPicker: {
				Color col = val.as<Color>();
				static_cast<CColorPicker*>(element)->value[0] = col.r / 255.f;
				static_cast<CColorPicker*>(element)->value[1] = col.g / 255.f;
				static_cast<CColorPicker*>(element)->value[2] = col.b / 255.f;
				static_cast<CColorPicker*>(element)->value[3] = col.a / 255.f;
				break;
			}
			case WidgetType::KeyBind:
				static_cast<CKeyBind*>(element)->set(val.as<bool>());
				break;
			case WidgetType::SliderInt:
				static_cast<CSliderInt*>(element)->value = val.as<int>();
				break;
			case WidgetType::SliderFloat:
				static_cast<CSliderFloat*>(element)->value = val.as<float>();
				break;
			case WidgetType::Combo:
				static_cast<CComboBox*>(element)->value = val.as<int>();
				break;
			case WidgetType::MultiCombo:
				static_cast<CMultiCombo*>(element)->value[index.value()] = val.as<bool>();
				break;
			case WidgetType::Input: {
				ZeroMemory(static_cast<CInputBox*>(element)->buf, 64);
				std::string inp = val.as<std::string>();
				memcpy(static_cast<CInputBox*>(element)->buf, inp.c_str(), inp.size());
				break;
			}
			default:
				Console->Error("unknown type");
			}
		}

		bool is_open() {
			return Menu->IsOpened();
		}

		std::vector<IBaseWidget*> get_binds() {
			return Menu->GetKeyBinds();
		}

		void element_update_list(sol::this_state state, IBaseWidget* element, std::vector<std::string> list) {
			switch (element->GetType()) {
			case WidgetType::Combo:
				return static_cast<CComboBox*>(element)->UpdateList(list);
			case WidgetType::MultiCombo:
				return static_cast<CMultiCombo*>(element)->UpdateList(list);
			}

			Console->Error("trying to update list of non listable element");
		}

		void element_set_callback(sol::this_state state, IBaseWidget* element, sol::protected_function func) {
			UILuaCallback_t cb(element, Lua->GetScriptID(GetCurrentScript(state)), func);
			element->lua_callbacks.push_back(cb);
			g_ui_lua_callbacks.push_back(cb); // track callback to easily remove them
		}
		 
		void element_set_visible(IBaseWidget* element, bool visible) {
			element->SetVisible(visible);
		}

		CMenuTab* tab(sol::this_state state, std::string name, sol::optional<DXImage> icon) {
			CMenuTab* tab = Menu->AddTab(name, icon.value_or(pic::tab::scripts));
			
			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->tabs.push_back(tab);

			return tab;
		}

		CMenuGroupbox* groupbox(sol::this_state state, std::string tab, std::string groupbox) {
			CMenuGroupbox* gb = Menu->AddGroupBox(tab, groupbox);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->groupboxes.push_back(gb);

			return gb;
		}

		IBaseWidget* checkbox(sol::this_state state, CMenuGroupbox* self, std::string name, sol::optional<bool> def) {
			IBaseWidget* elem = self->AddCheckBox(name, def.value_or(false));

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* label(sol::this_state state, CMenuGroupbox* self, std::string name) {
			IBaseWidget* elem = self->AddLabel(name);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* color_picker(sol::this_state state, CMenuGroupbox* self, std::string name, sol::optional<Color> default_color) {
			IBaseWidget* elem = self->AddColorPicker(name, default_color.value_or(Color()));

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* keybind(sol::this_state state, CMenuGroupbox* self, std::string name) {
			IBaseWidget* elem = self->AddKeyBind(name);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* slider_int(sol::this_state state, CMenuGroupbox* self, std::string name, int min_, int max_, int default_val, sol::optional<std::string> format) {
			IBaseWidget* elem = self->AddSliderInt(name, min_, max_, default_val, format.value_or("%d"));

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* slider_float(sol::this_state state, CMenuGroupbox* self, std::string name, float min_, float max_, float default_val, sol::optional<std::string> format) {
			IBaseWidget* elem = self->AddSliderFloat(name, min_, max_, default_val, format.value_or("%.2f"));

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* combo(sol::this_state state, CMenuGroupbox* self, std::string name, sol::variadic_args elements) {
			std::vector<std::string> vals;
			for (auto el : elements) {
				std::string s = el;
				vals.push_back(s);
			}
			IBaseWidget* elem = self->AddComboBox(name, vals);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* multicombo(sol::this_state state, CMenuGroupbox* self, std::string name, sol::variadic_args elements) {
			std::vector<std::string> vals;
			for (auto el : elements) {
				std::string s = el;
				vals.push_back(s);
			}
			IBaseWidget* elem = self->AddMultiCombo(name, vals);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}
	
		IBaseWidget* button(sol::this_state state, CMenuGroupbox* self, std::string name) {
			IBaseWidget* elem = self->AddButton(name);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}

		IBaseWidget* input(sol::this_state state, CMenuGroupbox* self, std::string name) {
			IBaseWidget* elem = self->AddInput(name);

			LuaScript_t* script = &Lua->scripts[Lua->GetScriptID(GetCurrentScript(state))];
			script->ui_elements.push_back(elem);

			return elem;
		}
	}

	namespace entity {
		sol::object get(sol::this_state state, int index, sol::optional<bool> is_userid) {
			if (is_userid.value_or(false))
				index = EngineClient->GetPlayerForUserID(index);

			CBaseEntity* result = EntityList->GetClientEntity(index);

			if (!result)
				return sol::nil;

			if (result->IsPlayer())
				return sol::make_object(state, reinterpret_cast<CBasePlayer*>(result));

			if (result->IsWeapon())
				return sol::make_object(state, reinterpret_cast<CBaseCombatWeapon*>(result));

			return sol::make_object(state, result);
		}

		sol::object from_handle(sol::this_state state, unsigned int handle) {
			CBaseEntity* result = EntityList->GetClientEntityFromHandle(handle);

			if (!result)
				return sol::nil;

			if (result->IsPlayer())
				return sol::make_object(state, reinterpret_cast<CBasePlayer*>(result));

			if (result->IsWeapon())
				return sol::make_object(state, reinterpret_cast<CBaseCombatWeapon*>(result));

			return sol::make_object(state, result);
		}

		CBasePlayer* get_local_player() {
			return reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(EngineClient->GetLocalPlayer()));
		}

		sol::table get_simulation_time(sol::this_state state, CBasePlayer* pl) {
			sol::table result(state, sol::new_table{});
			result[1] = pl->m_flSimulationTime();
			result[2] = pl->m_flOldSimulationTime();
			return result;
		}

		sol::object get_prop(sol::this_state state, CBaseEntity* ent, std::string prop_name) {
			static auto recvproxy_int32_to_int8 = Utils::PatternScan("client.dll", "55 8B EC 8B 45 08 8A 48 08 8B 45 10");
			static auto recvproxy_int32_to_int16 = Utils::PatternScan("client.dll", "55 8B EC 8B 45 08 66 8B 48 08 8B 45 10 66 89 08");

			auto prop = NetVars::FindProp(ent->GetClientClass()->m_pRecvTable, prop_name);

			if (prop.offset == 0)
				return sol::nil;

			switch (prop.prop->m_RecvType)
			{
			case DPT_Int: {
				if (prop.prop->m_ProxyFn) {
					if (prop.prop->m_ProxyFn == recvproxy_int32_to_int8)
						return sol::make_object(state, *reinterpret_cast<bool*>((uintptr_t)ent + prop.offset));
					else if (prop.prop->m_ProxyFn == recvproxy_int32_to_int16)
						return sol::make_object(state, *reinterpret_cast<short*>((uintptr_t)ent + prop.offset));
				}

				return sol::make_object(state, *reinterpret_cast<int*>((uintptr_t)ent + prop.offset));
			}
			case DPT_Float:
				return sol::make_object(state, *reinterpret_cast<float*>((uintptr_t)ent + prop.offset));
			case DPT_Vector:
			case DPT_VectorXY:
				return sol::make_object(state, *reinterpret_cast<Vector*>((uintptr_t)ent + prop.offset));
			case DPT_String:
				return sol::make_object(state, reinterpret_cast<char*>((uintptr_t)ent + prop.offset));
			case DPT_Array: {
				auto array_prop = prop.prop->m_pArrayProp[0];

				switch (array_prop.m_RecvType)
				{
				case DPT_Int:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<int*>((uintptr_t)ent + prop.offset)));
				case DPT_Float:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<float*>((uintptr_t)ent + prop.offset)));
				case DPT_Vector:
				case DPT_VectorXY:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<Vector*>((uintptr_t)ent + prop.offset)));
				case DPT_String:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<char**>((uintptr_t)ent + prop.offset)));
				}
			case DPT_DataTable: {
				auto dt_prop = prop.prop->m_pDataTable->m_pProps[0];

				switch (dt_prop.m_RecvType)
				{
				case DPT_Int:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<int*>((uintptr_t)ent + prop.offset)));
				case DPT_Float:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<float*>((uintptr_t)ent + prop.offset)));
				case DPT_Vector:
				case DPT_VectorXY:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<Vector*>((uintptr_t)ent + prop.offset)));
				case DPT_String:
					return sol::make_object(state, ArrayWrapper_Lua(reinterpret_cast<char**>((uintptr_t)ent + prop.offset)));
				}
			}
			}
			default:
				break;
			}

 			return sol::nil;
		}

		Vector obb_maxs(sol::this_state state, CBaseEntity* ent) {
			auto collidable = ent->GetCollideable();

			if (!collidable)
				return Vector();

			return collidable->OBBMaxs();
		}

		Vector obb_mins(sol::this_state state, CBaseEntity* ent) {
			auto collidable = ent->GetCollideable();

			if (!collidable)
				return Vector();

			return collidable->OBBMins();
		}

		Vector collision_origin(sol::this_state state, CBaseEntity* ent) {
			auto collidable = ent->GetCollideable();

			if (!collidable)
				return Vector();

			return collidable->GetCollisionOrigin();
		}

		ArrayWrapper_Lua<AnimationLayer> get_anim_layers(CBasePlayer* pla) {
			return ArrayWrapper_Lua(pla->GetAnimlayers());
		}

		bool is_player(CBaseEntity* ent) {
			return ent->IsPlayer();
		}

		bool is_weapon(CBaseEntity* ent) {
			return ent->IsWeapon();
		}

		float get_dormant_last_update(CBasePlayer* pl) {
			return ESPInfo[pl->EntIndex()].m_flLastUpdateTime;
		}

		uintptr_t ptr(CBaseEntity* ent) {
			return reinterpret_cast<uintptr_t>(ent);
		}

		int get_classid(CBaseEntity* ent) {
			auto cc = ent->GetClientClass();

			if (!cc)
				return 0;

			return cc->m_ClassID;
		}

		std::string get_classname(CBaseEntity* ent) {
			auto cc = ent->GetClientClass();

			if (!cc)
				return 0;

			return cc->m_pNetworkName;
		}

		player_info_t get_player_info(CBasePlayer* pl) {
			player_info_t pinfo;
			EngineClient->GetPlayerInfo(pl->EntIndex(), &pinfo);
			return pinfo;
		}

		bool is_bot(CBasePlayer* pl) {
			player_info_t pinfo;
			EngineClient->GetPlayerInfo(pl->EntIndex(), &pinfo);
			return pinfo.fakeplayer;
		}

		void set_icon(CBasePlayer* player, int level) {
			ESP::IconDisplay(player, level);
		}
	}

	namespace network {
		std::string get(std::string url, sol::optional<sol::table> headers, sol::optional<sol::protected_function> callback) {
			if (callback.has_value()) {
				// will implement async request later
			}
			else {
				std::vector<HttpHeader> _headers;
				
				if (headers.has_value()) {
					auto val = headers.value();

					for (auto& header : val) {
						_headers.push_back(HttpHeader{ header.first.as<std::string>(), header.second.as<std::string>() });
					}
				}

				return Requests->Create(k_EHTTPMethodGET, url, _headers);
			}
		}

	}

	namespace rage {
		CBasePlayer* get_antiaim_target() {
			return AntiAim->GetNearestTarget();
		}

		float get_exploit_charge() {
			return (float)ctx.tickbase_shift / (float)Exploits->MaxTickbaseShift();
		}

		bool is_defensive_active() {
			return Exploits->IsDefensiveActive();
		}

		int get_defensive_ticks() {
			return Exploits->GetDefensiveTicks();
		}

		void force_teleport() {
			Exploits->ForceTeleport();
		}

		void force_charge() {
			Exploits->ForceCharge();
		}

		bool is_shifting() {
			return Exploits->IsShifting();
		}

		float get_antiaim_yaw() {
			return AntiAim->base_yaw;
		}

		void override_tickbase_shift(int tb) {
			Exploits->LC_OverrideTickbase(tb);
		}
	}

	namespace materials {
		IMaterial* create(std::string mat_name, std::string mattype, sol::table kv) {
			KeyValues* mat_kv = new KeyValues(mattype.c_str());

			for (auto& p : kv) {
				if (p.second.get_type() == sol::type::number)
					mat_kv->SetFloat(((std::string)"$" + p.first.as<std::string>()).c_str(), p.second.as<float>());
				else if (p.second.get_type() == sol::type::string)
					mat_kv->SetString(((std::string)"$" + p.first.as<std::string>()).c_str(), p.second.as<std::string>().c_str());
			}

			return MaterialSystem->CreateMaterial(mat_name.c_str(), mat_kv);
		}

		IMaterial* find(std::string mat_name) {
			return MaterialSystem->FindMaterial(mat_name.c_str());
		}

		void draw_chams(IMaterial* material) {
			ModelRender->ForcedMaterialOverride(material);
			Chams->DrawModelExecute();
			ModelRender->ForcedMaterialOverride(nullptr);
		}

		Color mat_color_modulate(IMaterial* self, sol::optional<Color> newColor) {
			if (newColor.has_value()) {
				self->ColorModulate(newColor.value());
			}
			else {
				float r, g, b;
				self->GetColorModulation(&r, &g, &b);
				return Color(r * 255.f, g * 255.f, b * 255.f);
			}
		}

		float mat_alpha_modulate(IMaterial* self, sol::optional<float> newAlpha) {
			if (newAlpha.has_value()) {
				self->AlphaModulate(newAlpha.value());
			}
			else {
				return self->GetAlphaModulation();
			}
		}

		bool mat_var_flag(IMaterial* self, uint32_t varflag, sol::optional<bool> newValue) {
			if (newValue.has_value()) {
				self->SetMaterialVarFlag((MaterialVarFlags_t)varflag, newValue.value());
			}
			else {
				return self->GetMaterialVarFlag((MaterialVarFlags_t)varflag);
			}
		}
	}
}

void CLua::Setup() {
	std::filesystem::create_directory(std::filesystem::current_path().string() + "/at/scripts");
	std::filesystem::create_directory(std::filesystem::current_path().string() + "/at/scripts/cfg");

	lua = sol::state(sol::c_call<decltype(&LuaErrorHandler), &LuaErrorHandler>);
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::package, sol::lib::jit, sol::lib::ffi, sol::lib::bit32);

	lua["math"]["angle_diff"] = Math::AngleDiff;

	// usertypes
	lua.new_usertype<IBaseWidget>("ui_element_t", sol::no_constructor, 
		"set_callback", api::ui::element_set_callback,
		"visible", api::ui::element_set_visible,
		"type", api::ui::element_type,
		"get_name", api::ui::element_get_name,
		"get_mode", api::ui::element_get_mode,
		"get_key", api::ui::element_get_key,
		"get", api::ui::element_get,
		"set", api::ui::element_set,
		"update_list", api::ui::element_update_list
	);

	lua.new_usertype<Color>("color", sol::call_constructor, sol::constructors<Color(), Color(int), Color(int, int), Color(int, int, int), Color(int, int, int, int)>(),
		"r", &Color::r,
		"g", &Color::g,
		"b", &Color::b,
		"a", &Color::a,
		"as_int32", &Color::as_int32,
		"as_fraction", &Color::as_fraction,
		"alpha_modulate", &Color::alpha_modulate,
		"alpha_modulatef", &Color::alpha_modulatef,
		"lerp", &Color::lerp,
		"clone", &Color::clone
	);

	lua.new_usertype<Vector>("vector", sol::call_constructor, sol::constructors<Vector(), Vector(float, float), Vector(float, float, float)>(),
		"x", &Vector::x,
		"y", &Vector::y,
		"z", &Vector::z,
		"__add", &Vector::operator+,
		"__sub", &Vector::operator-,
		"__mul", &Vector::operator*,
		"__div", &Vector::operator/,
		"__eq", &Vector::operator==,
		"__len", &Vector::Q_Length,
		"__tostring", &api::vector::tostring,
		"length", &Vector::Q_Length,
		"length_sqr", &Vector::LengthSqr,
		"length2d", &Vector::Q_Length2D,
		"length2d_sqr", &Vector::Length2DSqr,
		"closest_ray_point", api::vector::closes_ray_point,
		"dist_to_ray", api::vector::dist_to_ray,
		"dot", &Vector::Dot,
		"cross", &Vector::Cross,
		"dist", &Vector::DistTo,
		"lerp", api::vector::lerp,
		"angles", api::vector::angles,
		"to_screen", api::vector::to_screen
	);

	lua.new_usertype<DXImage>("image_t", sol::no_constructor, 
		"width", &DXImage::width,
		"height", &DXImage::height
	);

	lua.new_usertype<player_info_t>("player_info_t", sol::no_constructor,
		"steamID64", &player_info_t::steamID64,
		"xuid_low", &player_info_t::xuid_low,
		"xuid_high", &player_info_t::xuid_high,
		"name", &player_info_t::szName,
		"userid", &player_info_t::userId,
		"szSteamID", &player_info_t::szSteamID,
		"steamID", &player_info_t::iSteamID,
		"bot", &player_info_t::fakeplayer
	);

	lua.new_usertype<CBaseEntity>("entity_t", sol::no_constructor,
		"ent_index", &CBaseEntity::EntIndex,
		"get_abs_origin", &CBasePlayer::GetAbsOrigin,
		"get_abs_angles", &CBasePlayer::GetAbsAngles,
		"obb_maxs", api::entity::obb_maxs,
		"obb_mins", api::entity::obb_mins,
		"collision_origin", api::entity::collision_origin,
		"is_player", api::entity::is_player,
		"is_weapon", api::entity::is_weapon,
		"ptr", api::entity::ptr,
		"get_classid", api::entity::get_classid,
		"get_classname", api::entity::get_classname,
		"__index", api::entity::get_prop
	);

	lua.new_usertype<CBaseCombatWeapon>("weapon_t", sol::no_constructor,
		"weapon_index", &CBaseCombatWeapon::m_iItemDefinitionIndex,
		"get_inaccuracy", &CBaseCombatWeapon::GetInaccuracy,
		"get_spread", &CBaseCombatWeapon::GetSpread,
		"shooting_weapon", &CBaseCombatWeapon::ShootingWeapon,
		"can_shoot", &CBaseCombatWeapon::CanShoot,
		"get_icon", &CBaseCombatWeapon::GetIcon,
		"is_grenade", &CBaseCombatWeapon::IsGrenade,
		"get_weapon_info", &CBaseCombatWeapon::GetWeaponInfo,
		"__index", api::entity::get_prop,
		sol::base_classes, sol::bases<CBaseEntity>()
	);

	lua.new_usertype<CCSGOPlayerAnimationState>("animstate_t", sol::no_constructor,
		"entity", &CCSGOPlayerAnimationState::pEntity,
		"active_weapon", &CCSGOPlayerAnimationState::pWeapon,
		"last_active_weapon", &CCSGOPlayerAnimationState::pWeaponLast,
		"last_update_time", &CCSGOPlayerAnimationState::flLastUpdateTime,
		"last_update_frame", &CCSGOPlayerAnimationState::nLastUpdateFrame,
		"last_update_increment", &CCSGOPlayerAnimationState::flLastUpdateIncrement,
		"eye_yaw", &CCSGOPlayerAnimationState::flEyeYaw,
		"eye_pitch", &CCSGOPlayerAnimationState::flEyePitch,
		"foot_yaw", &CCSGOPlayerAnimationState::flFootYaw,
		"last_foot_yaw", &CCSGOPlayerAnimationState::flLastFootYaw,
		"move_yaw", &CCSGOPlayerAnimationState::flMoveYaw,
		"move_yaw_ideal", &CCSGOPlayerAnimationState::flMoveYawIdeal,
		"move_yaw_current_to_ideal", &CCSGOPlayerAnimationState::flMoveYawCurrentToIdeal,
		"primary_cycle", &CCSGOPlayerAnimationState::flPrimaryCycle,
		"move_weight", &CCSGOPlayerAnimationState::flMoveWeight,
		"move_weight_smoothed", &CCSGOPlayerAnimationState::flMoveWeightSmoothed,
		"duck_amount", &CCSGOPlayerAnimationState::flDuckAmount,
		"duck_additional", &CCSGOPlayerAnimationState::flDuckAdditional,
		"recrouch_weight", &CCSGOPlayerAnimationState::flRecrouchWeight,
		"origin", &CCSGOPlayerAnimationState::vecOrigin,
		"last_origin", &CCSGOPlayerAnimationState::vecLastOrigin,
		"velocity", &CCSGOPlayerAnimationState::vecVelocity,
		"velocity_normalized", &CCSGOPlayerAnimationState::vecVelocityNormalized,
		"velocity_normalized_non_zero", &CCSGOPlayerAnimationState::vecVelocityNormalizedNonZero,
		"velocity_length_2d", &CCSGOPlayerAnimationState::flVelocityLenght2D,
		"velocity_z", &CCSGOPlayerAnimationState::flVelocityZ,
		"speed_normalized", &CCSGOPlayerAnimationState::flRunSpeedNormalized,
		"running_speed", &CCSGOPlayerAnimationState::flWalkSpeedNormalized,
		"ducking_speed", &CCSGOPlayerAnimationState::flCrouchSpeedNormalized,
		"duration_moving", &CCSGOPlayerAnimationState::flDurationMoving,
		"duration_still", &CCSGOPlayerAnimationState::flDurationStill,
		"on_ground", &CCSGOPlayerAnimationState::bOnGround,
		"landing", &CCSGOPlayerAnimationState::bLanding,
		"duration_in_air", &CCSGOPlayerAnimationState::flDurationInAir,
		"left_ground_height", &CCSGOPlayerAnimationState::flLeftGroundHeight,
		"hit_ground_weight", &CCSGOPlayerAnimationState::flHitGroundWeight,
		"walk_to_run_transition", &CCSGOPlayerAnimationState::flWalkToRunTransition,
		"in_air_smooth_value", &CCSGOPlayerAnimationState::flInAirSmoothValue,
		"min_aim_yaw", &CCSGOPlayerAnimationState::flAimYawMin,
		"max_aim_yaw", &CCSGOPlayerAnimationState::flAimYawMax,
		"min_pitch", &CCSGOPlayerAnimationState::flAimPitchMin,
		"max_pitch", &CCSGOPlayerAnimationState::flAimPitchMax,
		"animset_version", &CCSGOPlayerAnimationState::iAnimsetVersion
	);

	lua.new_usertype<AnimationLayer>("animlayer_t", sol::no_constructor,
		"client_blend", &AnimationLayer::m_bClientBlend,
		"layer_fade_out", &AnimationLayer::m_flBlendIn,
		"dispatched_studio_hdr", &AnimationLayer::m_pStudioHdr,
		"dispatched_src", &AnimationLayer::m_nDispatchSequence,
		"dispatched_dest", &AnimationLayer::m_nDispatchSequence_2,
		"order", &AnimationLayer::m_nOrder,
		"sequence", &AnimationLayer::m_nSequence,
		"prev_cycle", &AnimationLayer::m_flPrevCycle,
		"weight", &AnimationLayer::m_flWeight,
		"weight_delta_rate", &AnimationLayer::m_flWeightDeltaRate,
		"playback_rate", &AnimationLayer::m_flPlaybackRate,
		"cycle", &AnimationLayer::m_flCycle,
		"owner", &AnimationLayer::m_pOwner
	);

	lua.new_usertype<CBasePlayer>("player_t", sol::no_constructor, 
		"get_name", &CBasePlayer::GetName,
		"get_active_weapon", &CBasePlayer::GetActiveWeapon,
		"is_alive", &CBasePlayer::IsAlive,
		"is_enemy", &CBasePlayer::IsEnemy,
		"is_bot", api::entity::is_bot,
		"get_player_info", api::entity::get_player_info,
		"get_eye_position", &CBasePlayer::GetEyePosition,
		"get_bone_position", &CBasePlayer::GetBonePosition,
		"get_hitbox_position", &CBasePlayer::GetHitboxCenter,
		"get_animstate", &CBasePlayer::GetAnimstate,
		"get_animlayers", &api::entity::get_anim_layers,
		"get_simulation_time", api::entity::get_simulation_time,
		"get_dormant_last_update", &api::entity::get_dormant_last_update,
		"set_icon", api::entity::set_icon,
		"__index", api::entity::get_prop,
		sol::base_classes, sol::bases<CBaseEntity>()
	);

	lua.new_usertype<LagRecord>("lag_record_t", sol::no_constructor,
		"player", &LagRecord::player,
		"origin", &LagRecord::m_vecOrigin,
		"velocity", &LagRecord::m_vecVelocity,
		"mins", &LagRecord::m_vecMins,
		"maxs", &LagRecord::m_vecMaxs,
		"abs_angles", &LagRecord::m_vecAbsAngles,
		"view_angles", &LagRecord::m_angEyeAngles,
		"simulation_time", &LagRecord::m_flSimulationTime,
		"duck_amount", &LagRecord::m_flDuckAmout,
		"duck_speed", &LagRecord::m_flDuckSpeed,
		"choked_ticks", &LagRecord::m_nChokedTicks,
		"shifting_tickbase", &LagRecord::shifting_tickbase,
		"breaking_lag_compensation", &LagRecord::breaking_lag_comp
	);

	DEF_LUA_ARR(int);
	DEF_LUA_ARR(float);
	DEF_LUA_ARR(Vector);
	DEF_LUA_ARR(char*);
	DEF_LUA_ARR(AnimationLayer);

	lua.new_usertype<QAngle>("qangle", sol::call_constructor, sol::constructors<QAngle(), QAngle(float, float), QAngle(float, float, float)>(), 
		"pitch", &QAngle::pitch,
		"yaw", &QAngle::yaw,
		"roll", &QAngle::roll
	);

	lua.new_usertype<RegisteredShot_t>("shot_t", sol::no_constructor, 
		"client_shoot_pos", &RegisteredShot_t::client_shoot_pos,
		"vector_pos", &RegisteredShot_t::target_pos,
		"client_angle", &RegisteredShot_t::client_angle,
		"shot_tick", &RegisteredShot_t::shot_tick,
		"wanted_damage", &RegisteredShot_t::wanted_damage,
		"wanted_damagegroup", &RegisteredShot_t::wanted_damagegroup,
		"hitchance", &RegisteredShot_t::hitchance,
		"backtrack", &RegisteredShot_t::backtrack,
		"record", &RegisteredShot_t::record,
		"shoot_pos", &RegisteredShot_t::shoot_pos,
		"end_pos", &RegisteredShot_t::end_pos,
		"angle", &RegisteredShot_t::angle,
		"ack_tick", &RegisteredShot_t::ack_tick,
		"impacts", &RegisteredShot_t::impacts,
		"damage", &RegisteredShot_t::damage,
		"damagegroup", &RegisteredShot_t::damagegroup,
		"hit_point", &RegisteredShot_t::hit_point,
		"acked", &RegisteredShot_t::acked,
		"miss_reason", &RegisteredShot_t::miss_reason
	);

	lua.new_usertype<CMenuGroupbox>("groupbox_t", sol::no_constructor,
		"checkbox", api::ui::checkbox,
		"label", api::ui::label,
		"color_picker", api::ui::color_picker,
		"keybind", api::ui::keybind,
		"slider_int", api::ui::slider_int,
		"slider_float", api::ui::slider_float,
		"combo", api::ui::combo,
		"multicombo", api::ui::multicombo,
		"input", api::ui::input,
		"button", api::ui::button
	);

	lua.new_usertype<CUserCmd_lua>("user_cmd_t", sol::no_constructor,
		"command_number", &CUserCmd_lua::command_number,
		"tickcount", &CUserCmd_lua::tickcount,
		"move", &CUserCmd_lua::move,
		"viewangles", &CUserCmd_lua::viewangles,
		"buttons", &CUserCmd_lua::buttons,
		"random_seed", &CUserCmd_lua::random_seed,
		"override_defensive", &CUserCmd_lua::override_defensive,
		"allow_defensive", &CUserCmd_lua::allow_defensive,
		"set_move", &CUserCmd_lua::set_move
	);

	lua.new_usertype<CGameTrace>("trace_t", sol::no_constructor,
		"startpos", &CGameTrace::startpos,
		"endpos", &CGameTrace::endpos,
		"fraction", &CGameTrace::fraction,
		"allsolid", &CGameTrace::allsolid,
		"startsolid", &CGameTrace::startsolid,
		"hit_entity", &CGameTrace::hit_entity,
		"hitgroup", &CGameTrace::hitgroup
	);

	lua.new_usertype<CSVCMsg_VoiceData_Lua>("recv_voice_data_t", sol::no_constructor,
		"client", &CSVCMsg_VoiceData_Lua::client,
		"audible_mask", &CSVCMsg_VoiceData_Lua::audible_mask,
		"xuid", &CSVCMsg_VoiceData_Lua::xuid,
		"xuid_low", &CSVCMsg_VoiceData_Lua::xuid_low,
		"xuid_high", &CSVCMsg_VoiceData_Lua::xuid_high,
		"format", &CSVCMsg_VoiceData_Lua::format,
		"sequence_bytes", &CSVCMsg_VoiceData_Lua::sequence_bytes,
		"section_number", &CSVCMsg_VoiceData_Lua::section_number,
		"uncompressed_sample_offset", &CSVCMsg_VoiceData_Lua::uncompressed_sample_offset,
		"get_voice_data", &CSVCMsg_VoiceData_Lua::get_voice_data
	);

	lua.new_usertype<SharedVoiceData_t>("voice_data", sol::call_constructor, sol::constructors<SharedVoiceData_t()>(),
		"xuid_high", &SharedVoiceData_t::xuid_high,
		"sequence_bytes", &SharedVoiceData_t::sequence_bytes,
		"section_number", &SharedVoiceData_t::section_number,
		"uncompressed_sample_offset", &SharedVoiceData_t::uncompressed_sample_offset
	);

	lua.new_usertype<IMaterial>("material_t", sol::no_constructor,
		"get_name", &IMaterial::GetName,
		"color_modulate", api::materials::mat_color_modulate,
		"alpha_modulate", api::materials::mat_alpha_modulate,
		"var_flag", api::materials::mat_var_flag,
		"increment_ref_count", &IMaterial::IncrementReferenceCount,
		"decrement_ref_count", &IMaterial::DecrementReferenceCount
	);

	lua.new_usertype<IGameEvent>("game_event_t", sol::no_constructor,
		"get_name", &IGameEvent::GetName,
		"get_int", &IGameEvent::GetInt,
		"get_float", &IGameEvent::GetFloat,
		"get_bool", &IGameEvent::GetBool,
		"get_string", &IGameEvent::GetString,
		"__index", api::client::event_index
	);

	lua.new_usertype<LuaAntiAim_t>("antiaim_context_t", sol::no_constructor, 
		"yaw", &LuaAntiAim_t::override_yaw,
		"pitch", &LuaAntiAim_t::override_pitch,
		"yaw_offset", &LuaAntiAim_t::override_yaw_offset,
		"fakelag", &LuaAntiAim_t::override_fakelag,
		"desync", &LuaAntiAim_t::override_desync,
		"desync_angle", &LuaAntiAim_t::override_desync_angle,
		"desync_side", &LuaAntiAim_t::override_desync_side
	);

	// _G
	lua["print"] = api::print;
	lua["error"] = api::error;
	lua["print_raw"] = api::print_raw;
	lua["safe_call"] = api::safe_call;

	// client
	lua.create_named_table("client",
		"add_callback", api::client::add_callback,
		"unload_script", api::client::unload_script,
		"reload_script", api::client::reload_script
	);

	// entity
	lua.create_named_table("entity",
		"get", api::entity::get,
		"get_local_player", api::entity::get_local_player
	);

	// ui
	lua.create_named_table("ui",
		"tab", api::ui::tab,
		"groupbox", api::ui::groupbox,
		"find", api::ui::find,
		"is_open", api::ui::is_open,
		"get_binds", api::ui::get_binds
	);

	// global vars
	lua.new_usertype<CGlobalVarsBase>("global_vars_t", sol::no_constructor,
		"curtime", sol::readonly(&CGlobalVarsBase::curtime),
		"realtime", sol::readonly(&CGlobalVarsBase::realtime),
		"frametime", sol::readonly(&CGlobalVarsBase::frametime),
		"framecount", sol::readonly(&CGlobalVarsBase::framecount),
		"tickcount", sol::readonly(&CGlobalVarsBase::tickcount),
		"tickinterval", sol::readonly(&CGlobalVarsBase::interval_per_tick),
		"max_players", sol::readonly(&CGlobalVarsBase::max_clients),
		"is_connected", sol::readonly_property(&api::globals::is_connected),
		"is_in_game", sol::readonly_property(&api::globals::is_in_game),
		"choked_commands", sol::readonly_property(&api::globals::choked_commands),
		"commandack", sol::readonly_property(&api::globals::commandack),
		"commandack_prev", sol::readonly_property(&api::globals::commandack_prev),
		"last_outgoing_command", sol::readonly_property(&api::globals::last_outgoing_command),
		"server_tick", sol::readonly_property(&api::globals::server_tick),
		"client_tick", sol::readonly_property(&api::globals::client_tick),
		"delta_tick", sol::readonly_property(&api::globals::delta_tick),
		"clock_offset", sol::readonly_property(&api::globals::clock_offset)
	);
	lua["globals"] = GlobalVars;

	// render
	lua.create_named_table("render",
		"screen_size", api::render::screen_size,
		"camera_angles", api::render::camera_angles,
		"get_weapon_icon", api::render::get_weapon_icon,
		"add_font", api::render::add_font,
		"load_font", api::render::load_font,
		"load_image", api::render::load_image,
		"measure_text", api::render::measure_text,
		"blur", api::render::blur,
		"line", api::render::line,
		"poly", api::render::poly,
		"poly_line", api::render::poly_line,
		"rect", api::render::rect,
		"rect_outline", api::render::rect_outline,
		"gradient", api::render::gradient,
		"circle", api::render::circle,
		"circle_outline", api::render::circle_outline,
		"circle_3d", api::render::circle_3d,
		"circle_3d_outline", api::render::circle_3d_outline,
		"circle_gradient", api::render::circle_gradient,
		"texture", api::render::texture,
		"text", api::render::text,
		"vertex", api::render::vertex,
		"push_clip_rect", api::render::push_clip_rect,
		"pop_clip_rect", api::render::pop_clip_rect,
		"world_to_screen", api::vector::to_screen,
		"set_antialias", api::render::set_antialias
	);

	lua.create_named_table("common",
		"get_map_shortname", api::common::get_map_shortname
	);

	lua.create_named_table("files",
		"read", api::files::read,
		"write", api::files::write,
		"create_folder", api::files::create_folder,
		"exists", api::files::exists
	);

	lua.create_named_table("json", 
		"stringify", api::json::stringify,
		"parse", api::json::parse
	);

	// utils
	lua.create_named_table("utils",
		"random_seed", api::utils::random_seed,
		"random_int", api::utils::random_int,
		"random_float", api::utils::random_float,
		"console_exec", api::utils::console_exec,
		"create_interface", api::utils::create_interface,
		"pattern_scan", api::utils::pattern_scan,
		"trace_line", api::utils::trace_line,
		"trace_hull", api::utils::trace_hull,
		"is_key_pressed", api::utils::is_key_pressed,
		"get_mouse_position", api::utils::get_mouse_position,
		"send_voice_message", api::utils::send_voice_message
	);

	// network
	lua.create_named_table("network",
		"get", api::network::get
	);

	// rage
	lua.create_named_table("rage",
		"get_antiaim_target", api::rage::get_antiaim_target,
		"get_exploit_charge", api::rage::get_exploit_charge,
		"is_defensive_active", api::rage::is_defensive_active,
		"get_defensive_ticks", api::rage::get_defensive_ticks,
		"force_teleport", api::rage::force_teleport,
		"force_charge", api::rage::force_charge,
		"is_shifting", api::rage::is_shifting,
		"override_tickbase_shift", api::rage::override_tickbase_shift,
		"get_antiaim_yaw", api::rage::get_antiaim_yaw
	);

	lua.create_named_table("materials", 
		"create", api::materials::create,
		"find", api::materials::find,
		"draw_chams", api::materials::draw_chams
	);

	RefreshScripts();
	Config->lua_button->SetCallback(ScriptLoadButton);
	Config->lua_button_unload->SetCallback(ScriptUnloadButton);
	Config->lua_save->SetCallback(ScriptSaveButton);
	Config->lua_refresh->SetCallback([]() { Lua->RefreshScripts(); });
}

int CLua::GetScriptID(std::string name) {
	for (int i = 0; i < scripts.size(); i++) {
		if (scripts[i].name == name || scripts[i].ui_name == name)
			return i;
	}

	return -1;
}

std::string CLua::GetScriptPath(std::string name) {
	return GetScriptPath(GetScriptID(name));
}

std::string CLua::GetScriptPath( int id ) {
	if (id == -1)
		return  "";

	return scripts[id].path.string();
}

void CLua::LoadScript( int id ) {
	if (id == -1)
		return;

	if (scripts[id].loaded)
		return;

	const std::string path = GetScriptPath(id);

	if (path == "")
		return;

	LuaScript_t& script = scripts[id];

	script.loaded = true;
	script.env = new sol::environment(lua, sol::create, lua.globals());

	sol::environment& env = *script.env;

	bool error_load = false;

	auto load_result_func = [&error_load, script](lua_State* state, sol::protected_function_result result) {
		if (!result.valid()) {
			sol::error error = result;
			Console->Error(error.what());
			error_load = true;
		}

		return result;
	};

	lua.safe_script_file(path, env, load_result_func);

	LuaLoadConfig(&script);

	if (error_load)
	{
		UnloadScript(id);

		RefreshUI();
		return;
	}

	RefreshUI();
}

void CLua::UnloadScript(int id) {
	if (id == -1 )
		return;

	LuaScript_t& script = scripts[id];

	if (!script.loaded)
		return;

	LuaSaveConfig(&script);

	for (auto& current : hooks.getHooks(LUA_UNLOAD)) {
		if (current.scriptId == id)
			current.func();
	}

	for (auto element : script.ui_elements) {
		Menu->RemoveItem(element);
	}

	for (auto gb : script.groupboxes) {
		Menu->RemoveGroupBox(gb);
	}

	for (auto tab : script.tabs) {
		Menu->RemoveTab(tab);
	}

	script.ui_elements.clear();
	script.groupboxes.clear();
	script.tabs.clear();

	for (auto cb = g_ui_lua_callbacks.begin(); cb != g_ui_lua_callbacks.end();) {
		if (cb->script_id == id) {
			for (auto it = cb->ref->lua_callbacks.begin(); it != cb->ref->lua_callbacks.end();) {
				if (it->script_id == id) {
					it = cb->ref->lua_callbacks.erase(it);
					continue;
				}

				it++;
			}

			cb = g_ui_lua_callbacks.erase(cb);
			continue;
		}

		cb++;
	}

	hooks.unregisterHooks(id);
	script.loaded = false;
	delete script.env;
	script.env = nullptr;

	RefreshUI();
}

void CLua::ReloadAll() {
	hooks.removeAll();

	for (int i = 0; i < scripts.size(); i++) {
		LuaScript_t* script = &scripts[i];

		if (script->loaded) {
			UnloadScript(i);
			LoadScript(i);
		}
	}
}

void CLua::UnloadAll() {
	for (int i = 0; i < scripts.size(); i++) {
		LuaScript_t* script = &scripts[i];

		if (script->loaded) {
			UnloadScript(i);
		}
	}
}

std::vector<std::string> CLua::GetUIList() {
	std::vector<std::string> result;

	for (auto& script : scripts) {
		result.push_back(script.ui_name);
	}

	return result;
}

void CLua::RefreshScripts() {
	auto old_scripts = scripts;

	ScriptSaveButton();
	UnloadAll();
	scripts.clear();

	for (auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path().string() + "/at/scripts"))
	{
		if (entry.path().extension() == ".lua")
		{
			LuaScript_t script;

			script.path = entry.path();
			script.name = script.path.stem().string();
			script.loaded = false;

			bool was_loaded = false;

			for (auto& o_script : old_scripts) {
				if (o_script.name == script.name && o_script.loaded) {
					was_loaded = true;
					break;
				}
			}
			script.ui_name = was_loaded ? "* " + script.name : script.name;

			scripts.push_back(script);
		}
	}

	Config->lua_list->UpdateList(GetUIList());

	for (auto script : old_scripts) {
		if (script.loaded)
			LoadScript(GetScriptID(script.name));
	}
}

void CLua::RefreshUI() {
	for (auto& script : scripts) {
		script.ui_name = script.loaded ? "* " + script.name : script.name;
	}

	Config->lua_list->UpdateList(GetUIList());
}

std::vector<std::string> CLua::GetLoadedScripts() {
	std::vector<std::string> result;

	for (auto& script : scripts) {
		if (script.loaded)
			result.push_back(script.name);
	}

	return result;
}

CLua* Lua = new CLua;