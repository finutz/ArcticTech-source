#pragma once

#include <Windows.h>
#include <vector>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../ImGui/imgui.h"
#include "../Features/Lua/Sol.hpp"
#include "../SDK/Misc/Color.h"
#include "../SDK/Render.h"

class CMenuGroupbox;
class IBaseWidget;

struct UILuaCallback_t {
	IBaseWidget* ref;
	int script_id;
	sol::protected_function func;
};
typedef void(*tUiCallback)();

enum class WidgetType {
	Checkbox,
	SliderInt,
	SliderFloat,
	KeyBind,
	Label,
	ColorPicker,
	Combo,
	MultiCombo,
	Button,
	Input,
	Any // used for menu.find only
};

class IBaseWidget {
public:
	std::string name;
	CMenuGroupbox* parent = nullptr;
	IBaseWidget* additional = nullptr;
	IBaseWidget* parent_item = nullptr;
	bool visible = true;
	std::vector<UILuaCallback_t> lua_callbacks;
	std::vector<tUiCallback> callbacks;

	void SetCallback(tUiCallback callback) { 
		callbacks.emplace_back(callback);
		if (this->GetType() != WidgetType::Button)
			callback();
	};

	void SetVisible(bool visible_) {
		visible = visible_;

		if (parent_item && parent_item->GetType() == WidgetType::Label)
			parent_item->SetVisible(visible_);
	};

	virtual WidgetType GetType() = 0;
	virtual void Render() = 0;
};

class CKeyBind : public IBaseWidget {
	bool pressed_once = false;
public:
	int key = 0;
	int mode = 2;
	bool toggled = false;

	bool get();
	void set(bool n) { toggled = n; }; // for toggle mode only

	virtual WidgetType GetType() { return WidgetType::KeyBind; };
	virtual void Render();
};

class CCheckBox : public IBaseWidget {
public:
	bool value;

	bool get(bool use_keybind = true) {
		if (!use_keybind || !additional || additional->GetType() != WidgetType::KeyBind)
			return value;

		return value && reinterpret_cast<CKeyBind*>(additional)->get();
	};

	virtual WidgetType GetType() { return WidgetType::Checkbox; };
	virtual void Render();
};

class CSliderInt : public IBaseWidget {
public:
	int value;
	int min;
	int max;
	std::string format;
	ImGuiSliderFlags flags;

	int get() { return value; };

	virtual WidgetType GetType() { return WidgetType::SliderInt; };
	virtual void Render();
};

class CSliderFloat : public IBaseWidget {
public:
	float value;
	float min;
	float max;
	std::string format;
	ImGuiSliderFlags flags;

	float get() { return value; };

	virtual WidgetType GetType() { return WidgetType::SliderFloat; };
	virtual void Render();
};

class CLabel : public IBaseWidget {
public:

	virtual WidgetType GetType() { return WidgetType::Label; };
	virtual void Render();
};

class CColorPicker : public IBaseWidget {
public:
	float value[4];
	bool has_alpha = false;

	Color get() { return Color(value[0] * 255, value[1] * 255, value[2] * 255, value[3] * 255); };

	virtual WidgetType GetType() { return WidgetType::ColorPicker; };
	virtual void Render();
};

class CComboBox : public IBaseWidget {
public:
	std::vector<const char*> elements;
	int value = 0;

	int get() { return value; };
	std::string get_name() { 
		if (elements.size() == 0)
			return "";
		return elements[value];
	};
	void UpdateList(const std::vector<std::string>& new_el) { 
		for (auto st : elements)
			delete[] st;

		std::vector<const char*> elems;
		for (auto st : new_el) {
			char* buf = new char[st.size() + 1];
			memcpy(buf, st.c_str(), st.size());
			buf[st.size()] = 0;
			elems.push_back(buf);
		}

		elements = elems;
	};

	virtual WidgetType GetType() { return WidgetType::Combo; };
	virtual void Render();
};

class CMultiCombo : public IBaseWidget {
public:
	std::vector<const char*> elements;
	bool value[32];

	bool get(int i) { return value[i]; };
	void UpdateList(const std::vector<std::string>& new_el) { 
		for (auto st : elements)
			delete[] st;

		std::vector<const char*> elems;
		for (auto st : new_el) {
			char* buf = new char[st.size() + 1];
			memcpy(buf, st.c_str(), st.size());
			buf[st.size()] = 0;
			elems.push_back(buf);
		}

		elements = elems;
	};

	virtual WidgetType GetType() { return WidgetType::MultiCombo; };
	virtual void Render();
};

class CButton : public IBaseWidget {
public:
	virtual WidgetType GetType() { return WidgetType::Button; };
	virtual void Render();
};

class CInputBox : public IBaseWidget {
public:
	char buf[64];
	ImGuiInputTextFlags flags;

	const std::string get() { return std::string(buf); };

	virtual WidgetType GetType() { return WidgetType::Input; };
	virtual void Render();
};

struct CMenuTab;
class CMenuGroupbox {
public:
	int column = 0;
	float relative_size = 1.f;
	ImVec2 position;
	ImVec2 size;

	CMenuTab* parent;
	std::string name;
	std::vector<IBaseWidget*> widgets;

	void Render();

	CCheckBox*		AddCheckBox(const std::string& name, bool init = false);
	CSliderInt*		AddSliderInt(const std::string& name, int min, int max, int init, const std::string& format = "%d", ImGuiSliderFlags flags = 0);
	CSliderFloat*	AddSliderFloat(const std::string& name, float min, float max, float init, const std::string& format = "%.3f", ImGuiSliderFlags flags = 0);
	CKeyBind*		AddKeyBind(const std::string& name);
	CLabel*			AddLabel(const std::string& name);
	CColorPicker*	AddColorPicker(const std::string& name, Color color = Color(), bool has_alpha = true);
	CComboBox*		AddComboBox(const std::string& name, std::vector<std::string> items);
	CMultiCombo*	AddMultiCombo(const std::string& name, std::vector<std::string> items);
	CButton*		AddButton(const std::string& name);
	CInputBox*		AddInput(const std::string& name, const std::string& init = "", ImGuiInputTextFlags flags = 0);
};

struct IDirect3DTexture9;
struct CMenuTab {
	IDirect3DTexture9* icon;
	ImVec2 icon_size;
	std::string name;
	std::vector<CMenuGroupbox*> groupboxes;
};

namespace pic {
	inline DXImage logo;

	namespace tab {
		inline DXImage aimbot;
		inline DXImage antiaim;
		inline DXImage visuals;
		inline DXImage misc;
		inline DXImage players;
		inline DXImage skins;
		inline DXImage configs;
		inline DXImage scripts;
	}
}

class CMenu {
private:
	bool m_bMenuOpened = true;
	bool m_bIsInitialized = false;
	ImVec2 m_WindowSize;
	ImVec2 m_ItemSpacing;

	std::vector<CMenuTab*> m_Tabs;

	void RecalculateGroupboxes();
public:
	std::vector<IBaseWidget*> m_KeyBinds;

	bool			IsOpened() { return m_bMenuOpened; };
	bool			IsInitialized() { return m_bIsInitialized; };

	void			Setup();
	void			SetupUI();
	void			Release();
	void			Draw();
	bool			WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	CMenuTab*		AddTab(const std::string& tab, DXImage icon);
	CMenuGroupbox*	AddGroupBox(const std::string& tab, const std::string& groupbox, float relative_size = 1.f, int column = -1);

	CMenuTab*		FindTab(const std::string& name);
	CMenuGroupbox*	FindGroupbox(const std::string& tab, const std::string& groupbox);
	IBaseWidget*	FindItem(const std::string& tab, const std::string& groupbox, const std::string& name, WidgetType type = WidgetType::Any);
	std::vector<IBaseWidget*> GetKeyBinds();
	void			RemoveItem(IBaseWidget* item);
	void			RemoveGroupBox(CMenuGroupbox* gb);
	void			RemoveTab(CMenuTab* tab);
};

extern CMenu* Menu;