#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#include "../UI/UI.h"
#include "../Utils/json.hpp"

struct weapon_settings_t {
    std::string weapon_name;

    CMultiCombo* hitboxes{ nullptr };
    CMultiCombo* multipoints{ nullptr };
    CSliderInt* head_point_scale{ nullptr };
    CSliderInt* body_point_scale{ nullptr };
    CSliderInt* hitchance{ nullptr };
    CSliderInt* minimum_damage{ nullptr };
    CSliderInt* minimum_damage_override{ nullptr };
    CMultiCombo* auto_stop{ nullptr };
    CCheckBox* auto_scope{ nullptr };
    CSliderInt* delay_shot{ nullptr };
    CCheckBox* strict_hitchance{ nullptr };
    CSliderFloat* accuracy_boost{ nullptr };
    CCheckBox* aim_head_if_safe{ nullptr };

    weapon_settings_t() {}
    weapon_settings_t(const std::string& name) : weapon_name(name) {}

    inline void SetVisible(bool visible) {
        hitboxes->SetVisible(visible);
        multipoints->SetVisible(visible);
        head_point_scale->SetVisible(visible);
        body_point_scale->SetVisible(visible);
        hitchance->SetVisible(visible);
        minimum_damage->SetVisible(visible);
        minimum_damage_override->SetVisible(visible);
        auto_stop->SetVisible(visible);
        auto_scope->SetVisible(visible);
        delay_shot->SetVisible(visible);
        strict_hitchance->SetVisible(visible);
        accuracy_boost->SetVisible(visible);
        aim_head_if_safe->SetVisible(visible);
    };
};

struct config_t {
    struct ragebot_t {
        struct aimbot_t {
            CCheckBox* enabled;
            CComboBox* extrapolation;
            CCheckBox* dormant_aim;
            CKeyBind* dormant_aim_key;
            CCheckBox* extended_backtrack;
            CCheckBox* doubletap;
            CKeyBind* doubletap_key;
            CMultiCombo* doubletap_options;
            CCheckBox* hide_shots;
            CKeyBind* hide_shots_key;
            CKeyBind* force_teleport;
            CKeyBind* force_body_aim;
            CKeyBind* minimum_damage_override_key;
            CCheckBox* peek_assist;
            CColorPicker* peek_assist_color;
            CKeyBind* peek_assist_keybind;
            CCheckBox* show_aimpoints;
            CCheckBox* roll_resolver;
            CKeyBind* roll_resolver_key;
            CSliderInt* roll_angle;
        } aimbot;

        CComboBox* selected_weapon;

        struct weapons_t {
            weapon_settings_t global{ "Global" };
            weapon_settings_t awp{ "AWP" };
            weapon_settings_t autosniper{ "Auto" };
            weapon_settings_t scout{ "Scout" };
            weapon_settings_t deagle{ "Deagle" };
            weapon_settings_t revolver{ "Revolver" };
            weapon_settings_t pistol{ "Pistol" };
        } weapons;
    } ragebot;

    struct antiaim_t {
        struct anti_aimbot_angles_t {
            CComboBox* pitch;
            CComboBox* yaw;
            CCheckBox* yaw_jitter;
            CSliderInt* modifier_value;
            CKeyBind* manual_left;
            CKeyBind* manual_right;
            CMultiCombo* manual_options;
            CCheckBox* body_yaw;
            CMultiCombo* body_yaw_options;
            CSliderInt* body_yaw_limit;
            CKeyBind* inverter;
            CCheckBox* legacy_desync;
        } angles;

        struct fakelag_t {
            CCheckBox* enabled;
            CSliderInt* limit;
            CSliderInt* variability;
        } fakelag;

        struct misc_t {
            CKeyBind* fake_duck;
            CKeyBind* slow_walk;
            CComboBox* leg_movement;
            CCheckBox* jitter_move;
        } misc;
    } antiaim;

    struct visuals_t {
        struct esp_t {
            CCheckBox* enable;
            CCheckBox* dormant;
            CColorPicker* dormant_color;
            CCheckBox* bounding_box;
            CColorPicker* box_color;
            CCheckBox* health_bar;
            CCheckBox* custom_health;
            CColorPicker* custom_health_color;
            CCheckBox* name;
            CColorPicker* name_color;
            CMultiCombo* flags;
            CCheckBox* weapon_text;
            CColorPicker* weapon_text_color;
            CCheckBox* weapon_icon;
            CColorPicker* weapon_icon_color;
            CCheckBox* ammo;
            CColorPicker* ammo_color;
            CCheckBox* glow;
            CColorPicker* glow_color;
            CCheckBox* hitsound;
            CCheckBox* show_server_hitboxes;
            CCheckBox* shared_esp;
            CCheckBox* share_with_enemies;
            CCheckBox* hitmarker;
            CColorPicker* hitmarker_color;
            CCheckBox* damage_marker;
            CColorPicker* damage_marker_color;
        } esp;

        struct chams_t {
            CComboBox* chams_selector;

            CCheckBox* enemy;
            CColorPicker* enemy_color;
            CColorPicker* enemy_second_color;
            CCheckBox* enemy_invisible;
            CColorPicker* enemy_invisible_color;
            CComboBox* enemy_type;
            CSliderFloat* enemy_glow_thickness;

            CCheckBox* shot_chams;
            CColorPicker* shot_chams_color;
            CComboBox* shot_chams_type;
            CColorPicker* shot_chams_second_color;
            CSliderFloat* shot_chams_thickness;
            CSliderInt* shot_chams_duration;
            CMultiCombo* shot_chams_options;

            CCheckBox* local_player;
            CColorPicker* local_player_color;
            CComboBox* local_player_type;
            CColorPicker* local_player_second_color;
            CSliderFloat* local_glow_thickness;

            CCheckBox* attachments;
            CColorPicker* attachments_color;
            CComboBox* attachments_type;
            CColorPicker* attachments_second_color;
            CSliderFloat* attachments_glow_thickness;

            CCheckBox* viewmodel;
            CColorPicker* viewmodel_color;
            CComboBox* viewmodel_type;
            CColorPicker* viewmodel_second_color;
            CSliderFloat* viewmodel_glow_thickness;

            CCheckBox* disable_model_occlusion;
        } chams;

        struct other_esp_t {
            CCheckBox* radar;
            CMultiCombo* dropped_weapons;
            CColorPicker* dropped_weapons_color;
            CCheckBox* sniper_crosshair;
            CCheckBox* penetration_crosshair;
            CCheckBox* bomb;
            CColorPicker* bomb_color;
            CCheckBox* grenades;
            CCheckBox* molotov_radius;
            CColorPicker* molotov_radius_color;
            CCheckBox* grenade_trajecotry;
            CColorPicker* grenade_trajectory_color;
            CColorPicker* grenade_trajectory_hit_color;
            CCheckBox* grenade_proximity_warning;
            CColorPicker* grenade_predict_color;
            CMultiCombo* particles;
        } other_esp;

        struct effects_t {
            CSliderInt* fov;
            CSliderInt* fov_zoom;
            CSliderInt* fov_second_zoom;
            CMultiCombo* removals;
            CComboBox* remove_scope;
            CCheckBox* world_color_enable;
            CColorPicker* world_color;
            CCheckBox* props_color_enable;
            CColorPicker* props_color;
            CCheckBox* thirdperson;
            CKeyBind* thirdperson_bind;
            CSliderInt* thirdperson_distance;
            CSliderFloat* aspect_ratio;
            CCheckBox* client_impacts;
            CColorPicker* client_impacts_color;
            CCheckBox* server_impacts;
            CColorPicker* server_impacts_color;
            CSliderInt* impacts_duration;
            CComboBox* override_skybox;
            CCheckBox* override_fog;
            CColorPicker* fog_color;
            CSliderInt* fog_start;
            CSliderInt* fog_end;
            CSliderInt* fog_density;
            CCheckBox* preserve_killfeed;
            CCheckBox* custom_sun_direction;
            CSliderInt* sun_pitch;
            CSliderInt* sun_yaw;
            CSliderInt* sun_distance;
            CSliderInt* scope_blend;
            CSliderInt* viewmodel_scope_alpha;
        } effects;
    } visuals;

    struct misc_t {
        struct miscellaneous_t {
            CCheckBox* anti_untrusted;
            CSliderInt* automatic_grenade_release;
            CMultiCombo* logs;
            CMultiCombo* auto_buy;
            CCheckBox* filter_console;
            CCheckBox* clantag;
            CCheckBox* ad_block;
        } miscellaneous;

        struct movement_t {
            CCheckBox* auto_jump;
            CCheckBox* auto_strafe;
            CMultiCombo* compensate_throwable;
            CSliderInt* auto_strafe_smooth;
            CCheckBox* edge_jump;
            CKeyBind* edge_jump_key;
            CCheckBox* infinity_duck;
            CCheckBox* quick_stop;
        } movement;
    } misc;

    struct skinchanger_t {
        CCheckBox* override_knife;
        CCheckBox* mask_changer;
        CCheckBox* override_agent;
        CComboBox* knife_model; 
        CComboBox* current_profile;
        CComboBox* paint_kits;
        CComboBox* glove_paint_kits;
        CComboBox* mask_changer_models;
        CComboBox* agent_model_ct;
        CComboBox* agent_model_t;
    } skins;
};

extern config_t config;

struct config_item_t {
    IBaseWidget* item;
    std::string name;
};

class CConfig {
    std::vector<config_item_t> items;

    inline void add(IBaseWidget* item) { 
        items.emplace_back(config_item_t{ item, item->parent->name + "_" + item->name + "_" + std::to_string(static_cast<int>(item->GetType())) });
    };

public:
    CComboBox* lua_list;
    CButton* lua_button;
    CButton* lua_button_unload;
    CButton* lua_refresh;
    CButton* lua_save;

    CComboBox* config_list;
    CInputBox* config_name;
    CInputBox* knife_name;
    CButton* load_button;
    CButton* save_button;
    CButton* delete_button;
    CButton* import_button;
    CButton* export_button;

    void parse(nlohmann::json& cfg);
    nlohmann::json dump();

    CConfig() {
        std::filesystem::create_directory(std::filesystem::current_path().string() + "/at");
    }

    std::vector<std::string> GetAllConfigs() {
        std::vector<std::string> result;

        for (const auto& file : std::filesystem::directory_iterator(std::filesystem::current_path().string() + "/at"))
            if (file.path().extension() == ".cfg") {
                std::string fname = file.path().stem().string();
                char* cfg_name = new char[fname.size()];
                std::strcpy(cfg_name, fname.c_str());
                result.push_back(cfg_name);
            }

        return result;
    }

    void Init();
};

extern CConfig* Config;