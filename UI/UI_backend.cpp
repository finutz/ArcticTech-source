#include "UI.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_win32.h"
#include "../ImGui/backends/imgui_impl_dx9.h"
#include "../ImGui/imgui_internal.h"
#include <d3d9.h>
#include <tchar.h>

#include "../ImGui/imgui_settings.h"

#include "logo.h"
#include "mulish_font.h"
#include "icons.h"

#include "../SDK/Interfaces.h"

CMenu* Menu = new CMenu;

namespace font {
    ImFont* general = nullptr;
    ImFont* tab = nullptr;
}

static ImGuiIO* im_io;

void CMenu::Setup() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    im_io = &ImGui::GetIO();

    ImFontConfig cfg;

    font::general = im_io->Fonts->AddFontFromMemoryTTF(mulish, sizeof(mulish), 19.f, &cfg, im_io->Fonts->GetGlyphRangesCyrillic());
    font::tab = im_io->Fonts->AddFontFromMemoryTTF(mulish, sizeof(mulish), 15.f, &cfg, im_io->Fonts->GetGlyphRangesCyrillic());

    im_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    im_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    D3DDEVICE_CREATION_PARAMETERS creationParameters = { };
    if (FAILED(DirectXDevice->GetCreationParameters(&creationParameters)))
        return;

    // store window pointer
    HWND hWindow = creationParameters.hFocusWindow;
    if (hWindow == nullptr)
        return;

    ImGui_ImplWin32_Init(hWindow);
    ImGui_ImplDX9_Init(DirectXDevice);

    pic::logo = Render->LoadImageFromMemory(ui_logo, sizeof(ui_logo), Vector2(128, 128));
    pic::tab::aimbot = Render->LoadImageFromMemory(aimbot, sizeof(aimbot), Vector2(14, 14));
    pic::tab::antiaim = Render->LoadImageFromMemory(anti_aimbot, sizeof(anti_aimbot), Vector2(14.74f, 14.f));
    pic::tab::visuals = Render->LoadImageFromMemory(visuals, sizeof(visuals), Vector2(14, 9.33f));
    pic::tab::misc = Render->LoadImageFromMemory(misc, sizeof(misc), Vector2(14, 11.74f));
    pic::tab::players = Render->LoadImageFromMemory(players, sizeof(players), Vector2(8.75f, 14.f));
    pic::tab::skins = Render->LoadImageFromMemory(skins, sizeof(skins), Vector2(16, 16));
    pic::tab::configs = Render->LoadImageFromMemory(configs, sizeof(configs), Vector2(13.4f, 14.f));
    pic::tab::scripts = Render->LoadImageFromMemory(scripts, sizeof(scripts), Vector2(15, 12));

    m_WindowSize = ImVec2(950, 750);
    m_ItemSpacing = ImVec2(24, 24);

    SetupUI();

    m_bIsInitialized = true;
}

void CMenu::Release() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::Shutdown();
}

void CMenu::Draw() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool insert_pressed = false;

    if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
        if (!insert_pressed) {
            m_bMenuOpened = !m_bMenuOpened;
            insert_pressed = true;
        }
    }
    else {
        insert_pressed = false;
    }

    if (m_bMenuOpened) {
        ImGui::GetStyle().ItemSpacing = ImVec2(24, 24);
        ImGui::GetStyle().WindowPadding = ImVec2(0, 0);
        ImGui::GetStyle().ScrollbarSize = 8.f;

        ImGui::SetNextWindowSize(m_WindowSize);

        ImGui::Begin("MENU", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
        {
            const ImVec2& window_pos = ImGui::GetWindowPos();
            const ImVec2& window_size = ImGui::GetContentRegionMax();
            const ImVec2& item_spacing = ImGui::GetStyle().ItemSpacing;

            m_WindowSize = window_size;
            m_ItemSpacing = item_spacing;

            ImGui::GetBackgroundDrawList()->AddRectFilled(window_pos + ImVec2(0, 0), window_pos + ImVec2(window_size), ImGui::GetColorU32(c::background::bg), c::background::rounding);

            ImGui::GetWindowDrawList()->AddRectFilled(window_pos + ImVec2(item_spacing), window_pos + ImVec2(181, (window_size.y - item_spacing.y)), ImGui::GetColorU32(c::child::bg), c::child::rounding);

            ImGui::GetWindowDrawList()->AddRectFilled(window_pos + ImVec2(181 + item_spacing.x, item_spacing.y), window_pos + ImVec2((window_size.x - item_spacing.x), (item_spacing.y + 58)), ImGui::GetColorU32(c::child::bg), c::child::rounding);

            ImGui::GetWindowDrawList()->AddImage(pic::logo.texture, window_pos + ImVec2(181 / 2 - 128 / 2 + item_spacing.x / 2, item_spacing.y - 16), window_pos + ImVec2(181 / 2 + 128 / 2 + item_spacing.x / 2, item_spacing.y - 16 + 128), ImVec2(0, 0), ImVec2(1, 1));

            static int tabs = 0;

            ImGui::SetCursorPos(ImVec2((item_spacing.x * 2), (48 + (item_spacing.y * 3))));

            ImGui::BeginGroup();
            {
                for (int i = 0; i < m_Tabs.size(); i++) {
                    CMenuTab* tab = m_Tabs[i];
                    if (ImGui::Tab(i == tabs, tab->icon, tab->name.c_str(), ImVec2(133 - ImGui::GetStyle().ItemSpacing.x, 44), tab->icon_size))
                        tabs = i;
                }
            }
            ImGui::EndGroup();

            static float tab_alpha = 0.f; 
            static float tab_add; 
            static int active_tab = 0;

            tab_alpha = ImClamp(tab_alpha + (4.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
            if (tab_alpha == 0.f && tab_add == 0.f) 
                active_tab = tabs;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * ImGui::GetStyle().Alpha);

            for (auto group : m_Tabs[active_tab]->groupboxes) {
                group->Render();
            }

            ImGui::PopStyleVar();

        }
        ImGui::End();
    }

    ImGui::Render();

    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CMenu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        return true;
    default:
        return ImGui::GetIO().WantTextInput;
    }
}

void CMenu::RecalculateGroupboxes() {
    const float groupbox_width = (m_WindowSize.x / 2) - ((181 / 2) + (m_ItemSpacing.x + 12));
    const ImVec2 base_position((181 + m_ItemSpacing.x), (m_ItemSpacing.y * 2) + 58);
    const ImVec2 container_size(groupbox_width * 2 + m_ItemSpacing.x, m_WindowSize.y - base_position.y - m_ItemSpacing.y);

    // size.x - (181 + (spacing.x + 24)))

    for (auto tab : m_Tabs) {
        std::vector<CMenuGroupbox*>& groupboxes = tab->groupboxes;
        const float gb_width = groupboxes.size() > 1 ? groupbox_width : (m_WindowSize.x - (181 + m_ItemSpacing.x + 24));

        float total_relative[2] = { 0.f, 0.f };
        int n_groupboxes[2] = { 0, 0 };
        for (auto gb : groupboxes) {
            total_relative[gb->column] += gb->relative_size;
            n_groupboxes[gb->column]++;
        }

        float available_space[2] = { container_size.y - m_ItemSpacing.y * (n_groupboxes[0] - 1), container_size.y - m_ItemSpacing.y * (n_groupboxes[1] - 1) };
        float current_position[2] = { 0.f, 0.f };

        for (auto gb : groupboxes) {
            gb->position.y = base_position.y + current_position[gb->column];
            gb->position.x = base_position.x + (gb_width + m_ItemSpacing.x) * gb->column;
            gb->size.x = gb_width;
            gb->size.y = available_space[gb->column] * (gb->relative_size / total_relative[gb->column]);

            current_position[gb->column] += gb->size.y + m_ItemSpacing.y;
        }
    }
}

CMenuTab* CMenu::AddTab(const std::string& tab, DXImage icon) {
    CMenuTab* result = new CMenuTab;

    result->icon = icon.texture;
    result->icon_size = ImVec2(icon.width, icon.height);
    result->name = tab;

    m_Tabs.push_back(result);

    return result;
}

CMenuGroupbox* CMenu::AddGroupBox(const std::string& tab, const std::string& groupbox, float realtive_size, int column) {
    CMenuTab* _tab = Menu->FindTab(tab);

    if (!_tab)
        return nullptr;

    CMenuGroupbox* gb = new CMenuGroupbox;

    if (column == -1) {
        int n_columns[2]{ 0, 0 };

        for (auto gb : _tab->groupboxes) {
            n_columns[gb->column]++;
        }

        gb->column = (n_columns[0] <= n_columns[1]) ? 0 : 1;
    }
    else {
        gb->column = column;
    }

    gb->name = groupbox;
    gb->parent = _tab;

    _tab->groupboxes.push_back(gb);

    RecalculateGroupboxes();

    return gb;
}

void CMenuGroupbox::Render() {
    ImGui::SetCursorPos(position);

    ImGui::BeginGroup();
    ImGui::BeginChild(name.c_str(), size);

    for (int i = 0; i < widgets.size(); i++) {
        auto el = widgets[i];
        if (!el || !el->visible || el->GetType() == WidgetType::ColorPicker || el->GetType() == WidgetType::KeyBind)
            continue;

        el->Render();
    }

    ImGui::EndChild();
    ImGui::EndGroup();
}

bool CKeyBind::get() {
    if (key == 0)
        return false;

    if (mode == 2)
        return true;

    if (GetAsyncKeyState(key) & 0x8000 && !EngineClient->Con_IsVisible() && EngineClient->IsActiveApp()) {
        if (!pressed_once) {
            pressed_once = true;
            toggled = !toggled;
        }
    }
    else {
        pressed_once = false;
    }

    if (mode == 1) {
        return toggled;
    }
    else {
        return (GetAsyncKeyState(key) & 0x8000) && EngineClient->IsActiveApp();
    }
};

void CCheckBox::Render() {
    if (ImGui::Checkbox(name.c_str(), &value)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }

    if (additional) {
        ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, (ImGui::GetStyle().ItemSpacing.y * 2) + 4));
        additional->Render();
    }
}

void CSliderInt::Render() {
    if (ImGui::SliderInt(name.c_str(), &value, min, max, format.c_str(), flags)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CSliderFloat::Render() {
    if (ImGui::SliderFloat(name.c_str(), &value, min, max, format.c_str(), flags)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CKeyBind::Render() {
    if (ImGui::Keybind(name.c_str(), &key, &mode, false)) {
        if (key == VK_ESCAPE)
            key = 0;

        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CLabel::Render() {
    ImGui::Text(name.c_str());

    if (additional) {
        ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(0, (ImGui::GetStyle().ItemSpacing.y * 2) + 4));
        additional->Render();
    }
}

void CColorPicker::Render() {
    if (ImGui::ColorEdit4(name.c_str(), value, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | (has_alpha ? 0 : ImGuiColorEditFlags_NoAlpha))) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CComboBox::Render() {
    if (ImGui::Combo(name.c_str(), &value, elements.data(), static_cast<int>(elements.size()), 5, ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CMultiCombo::Render() {
    if (ImGui::MultiCombo(name.c_str(), value, elements.data(), elements.size(), ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CButton::Render() {
    if (ImGui::Button(name.c_str())) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

void CInputBox::Render() {
    if (ImGui::TextField(name.c_str(), nullptr, buf, 64, flags)) {
        for (auto& cb : callbacks)
            cb();
        for (auto lcb : lua_callbacks)
            lcb.func();
    }
}

CMenuTab* CMenu::FindTab(const std::string& name) {
    for (auto tab : m_Tabs)
        if (tab->name == name)
            return tab;

    return nullptr;
}

CMenuGroupbox* CMenu::FindGroupbox(const std::string& tab, const std::string& groupbox) {
    CMenuTab* _tab = FindTab(tab);

    if (!_tab)
        return nullptr;

    for (auto gb : _tab->groupboxes) {
        if (gb->name == groupbox)
            return gb;
    }

    return nullptr;
}

IBaseWidget* CMenu::FindItem(const std::string& tab, const std::string& groupbox, const std::string& name, WidgetType type) {
    CMenuGroupbox* gb = FindGroupbox(tab, groupbox);

    if (!gb)
        return nullptr;

    for (auto item : gb->widgets)
        if (item->name == name && ((type == WidgetType::Any && item->GetType() != WidgetType::Label) || type == item->GetType()))
            return item;

    return nullptr;
}

std::vector<IBaseWidget*> CMenu::GetKeyBinds() {
    return m_KeyBinds;
}

void CMenu::RemoveItem(IBaseWidget* widget) {
    CMenuGroupbox* gb = widget->parent;

    if (widget->GetType() == WidgetType::KeyBind) {
        for (auto it = m_KeyBinds.begin(); it != m_KeyBinds.end();) {
            if (*it == widget) {
                it = m_KeyBinds.erase(it);
                continue;
            }

            it++;
        }
    }

    for (auto it = gb->widgets.begin(); it != gb->widgets.end(); it++) {
        if (*it == widget) {
            if (widget->GetType() == WidgetType::MultiCombo || widget->GetType() == WidgetType::Combo) {
                CComboBox* box = reinterpret_cast<CComboBox*>(widget);

                for (auto st : box->elements)
                    delete[] st;
            }

            gb->widgets.erase(it);

            delete widget;
            return;
        }
    }
}

void CMenu::RemoveGroupBox(CMenuGroupbox* gb) {
    for (auto it = gb->parent->groupboxes.begin(); it != gb->parent->groupboxes.end(); it++) {
        if (*it == gb) {
            gb->parent->groupboxes.erase(it);

            delete gb;
            return;
        }
    }
}

void CMenu::RemoveTab(CMenuTab* tab) {
    for (auto it = m_Tabs.begin(); it != m_Tabs.end(); it++) {
        if (*it == tab) {
            m_Tabs.erase(it);

            delete tab;
            return;
        }
    }
}

CCheckBox* CMenuGroupbox::AddCheckBox(const std::string& name, bool init) {
    CCheckBox* item = new CCheckBox;

    item->name = name;
    item->parent = this;
    item->value = init;

    widgets.push_back(item);

    return item;
}

CSliderInt* CMenuGroupbox::AddSliderInt(const std::string& name, int min, int max, int init, const std::string& format, ImGuiSliderFlags flags) {
    CSliderInt* item = new CSliderInt;

    item->name = name;
    item->parent = this;
    item->min = min;
    item->max = max;
    item->value = init;
    item->format = format;
    item->flags = flags;

    widgets.push_back(item);

    return item;
}

CSliderFloat* CMenuGroupbox::AddSliderFloat(const std::string& name, float min, float max, float init, const std::string& format, ImGuiSliderFlags flags) {
    CSliderFloat* item = new CSliderFloat;

    item->name = name;
    item->parent = this;
    item->min = min;
    item->max = max;
    item->value = init;
    item->format = format;
    item->flags = flags;

    widgets.push_back(item);

    return item;
}

CKeyBind* CMenuGroupbox::AddKeyBind(const std::string& name) {
    IBaseWidget* parent_item = Menu->FindItem(parent->name, this->name, name);

    if (!parent_item)
        parent_item = AddLabel(name);

    CKeyBind* item = new CKeyBind;

    item->name = name;
    item->parent = this;
    item->parent_item = parent_item;
    parent_item->additional = item;

    widgets.push_back(item);
    Menu->m_KeyBinds.push_back(item);

    return item;
}

CLabel* CMenuGroupbox::AddLabel(const std::string& name) {
    CLabel* item = new CLabel;

    item->name = name;
    item->parent = this;

    widgets.push_back(item);

    return item;
}

CColorPicker* CMenuGroupbox::AddColorPicker(const std::string& name, Color init, bool has_alpha) {
    IBaseWidget* parent_item = Menu->FindItem(parent->name, this->name, name);

    if (!parent_item)
        parent_item = AddLabel(name);

    CColorPicker* item = new CColorPicker;

    item->name = name;
    item->parent = this;
    item->parent_item = parent_item;
    parent_item->additional = item;
    item->value[0] = init.r / 255.f;
    item->value[1] = init.g / 255.f;
    item->value[2] = init.b / 255.f;
    item->value[3] = init.a / 255.f;
    item->has_alpha = has_alpha;

    widgets.push_back(item);

    return item;
}

CComboBox* CMenuGroupbox::AddComboBox(const std::string& name, std::vector<std::string> items) {
    CComboBox* item = new CComboBox;

    item->name = name;
    item->parent = this;

    std::vector<const char*> elems;
    for (auto st : items) {
        char* buf = new char[st.size() + 1];
        memcpy(buf, st.c_str(), st.size());
        buf[st.size()] = 0;
        elems.push_back(buf);
    }
    item->elements = elems;

    widgets.push_back(item);

    return item;
}

CMultiCombo* CMenuGroupbox::AddMultiCombo(const std::string& name, std::vector<std::string> items) {
    CMultiCombo* item = new CMultiCombo;

    item->name = name;
    item->parent = this;

    std::vector<const char*> elems;
    for (auto st : items) {
        char* buf = new char[st.size() + 1];
        memcpy(buf, st.c_str(), st.size());
        buf[st.size()] = 0;
        elems.push_back(buf);
    }
    item->elements = elems;

    widgets.push_back(item);

    return item;
}

CButton* CMenuGroupbox::AddButton(const std::string& name) {
    CButton* item = new CButton;

    item->name = name;
    item->parent = this;

    widgets.push_back(item);

    return item;
}

CInputBox* CMenuGroupbox::AddInput(const std::string& name, const std::string& init, ImGuiInputTextFlags flags) {
    CInputBox* item = new CInputBox;

    item->name = name;
    item->parent = this;
    item->flags = flags;
    
    ZeroMemory(item->buf, 64);
    std::memcpy(item->buf, init.c_str(), init.size());

    widgets.push_back(item);

    return item;
}