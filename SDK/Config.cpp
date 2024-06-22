#include "Config.h"

#include "../Utils/Utils.h"
#include "../Utils/Console.h"
#include "../Features/Lua/Bridge/Bridge.h"

config_t config;
CConfig* Config = new CConfig;

// callbacks
void on_config_list_changed() {
    std::string cfg_name = Config->config_list->get_name();
    memcpy(Config->config_name->buf, cfg_name.c_str(), cfg_name.size() + 1);

    Config->config_list->UpdateList(Config->GetAllConfigs());
};

void on_load() {
    std::string name = Config->config_name->get();

    if (name == "")
        return;

    const std::string file_path = std::filesystem::current_path().string() + "/at/" + name + ".cfg";
    if (!std::filesystem::exists(file_path)) {
        Utils::Print("[arctictech] config doesnt exists: %s\n", name);
        return;
    }

    std::ifstream file(file_path);
    nlohmann::json config_json;
    file >> config_json;

    Config->parse(config_json);
}

void on_save() {
    std::string name = Config->config_name->get();

    if (name == "")
        return;

    const std::string file_path = std::filesystem::current_path().string() + "/at/" + name + ".cfg";

    std::ofstream file(file_path);
    nlohmann::json config_json = Config->dump();

    file << config_json;
};

void on_delete() {};
void on_import() {};
void on_export() {};

void CConfig::Init() {
    add(config.ragebot.aimbot.enabled);
    add(config.ragebot.aimbot.extrapolation);
    add(config.ragebot.aimbot.dormant_aim);
    add(config.ragebot.aimbot.dormant_aim_key);
    add(config.ragebot.aimbot.extended_backtrack);
    add(config.ragebot.aimbot.doubletap);
    add(config.ragebot.aimbot.doubletap_key);
    add(config.ragebot.aimbot.doubletap_options);
    add(config.ragebot.aimbot.hide_shots);
    add(config.ragebot.aimbot.hide_shots_key);
    add(config.ragebot.aimbot.force_teleport);
    add(config.ragebot.aimbot.force_body_aim);
    add(config.ragebot.aimbot.minimum_damage_override_key);
    add(config.ragebot.aimbot.peek_assist);
    add(config.ragebot.aimbot.peek_assist_color);
    add(config.ragebot.aimbot.peek_assist_keybind);
    add(config.ragebot.aimbot.show_aimpoints);
    add(config.ragebot.aimbot.roll_resolver);
    add(config.ragebot.aimbot.roll_resolver_key);
    add(config.ragebot.aimbot.roll_angle);
    add(config.ragebot.weapons.global.hitboxes);
    add(config.ragebot.weapons.global.multipoints);
    add(config.ragebot.weapons.global.head_point_scale);
    add(config.ragebot.weapons.global.body_point_scale);
    add(config.ragebot.weapons.global.hitchance);
    add(config.ragebot.weapons.global.minimum_damage);
    add(config.ragebot.weapons.global.minimum_damage_override);
    add(config.ragebot.weapons.global.auto_stop);
    add(config.ragebot.weapons.global.auto_scope);
    add(config.ragebot.weapons.global.delay_shot);
    add(config.ragebot.weapons.global.strict_hitchance);
    add(config.ragebot.weapons.global.accuracy_boost);
    add(config.ragebot.weapons.global.aim_head_if_safe);
    add(config.ragebot.weapons.awp.hitboxes);
    add(config.ragebot.weapons.awp.multipoints);
    add(config.ragebot.weapons.awp.head_point_scale);
    add(config.ragebot.weapons.awp.body_point_scale);
    add(config.ragebot.weapons.awp.hitchance);
    add(config.ragebot.weapons.awp.minimum_damage);
    add(config.ragebot.weapons.awp.minimum_damage_override);
    add(config.ragebot.weapons.awp.auto_stop);
    add(config.ragebot.weapons.awp.auto_scope);
    add(config.ragebot.weapons.awp.delay_shot);
    add(config.ragebot.weapons.awp.strict_hitchance);
    add(config.ragebot.weapons.awp.accuracy_boost);
    add(config.ragebot.weapons.awp.aim_head_if_safe);
    add(config.ragebot.weapons.autosniper.hitboxes);
    add(config.ragebot.weapons.autosniper.multipoints);
    add(config.ragebot.weapons.autosniper.head_point_scale);
    add(config.ragebot.weapons.autosniper.body_point_scale);
    add(config.ragebot.weapons.autosniper.hitchance);
    add(config.ragebot.weapons.autosniper.minimum_damage);
    add(config.ragebot.weapons.autosniper.minimum_damage_override);
    add(config.ragebot.weapons.autosniper.auto_stop);
    add(config.ragebot.weapons.autosniper.auto_scope);
    add(config.ragebot.weapons.autosniper.delay_shot);
    add(config.ragebot.weapons.autosniper.strict_hitchance);
    add(config.ragebot.weapons.autosniper.accuracy_boost);
    add(config.ragebot.weapons.autosniper.aim_head_if_safe);
    add(config.ragebot.weapons.scout.hitboxes);
    add(config.ragebot.weapons.scout.multipoints);
    add(config.ragebot.weapons.scout.head_point_scale);
    add(config.ragebot.weapons.scout.body_point_scale);
    add(config.ragebot.weapons.scout.hitchance);
    add(config.ragebot.weapons.scout.minimum_damage);
    add(config.ragebot.weapons.scout.minimum_damage_override);
    add(config.ragebot.weapons.scout.auto_stop);
    add(config.ragebot.weapons.scout.auto_scope);
    add(config.ragebot.weapons.scout.delay_shot);
    add(config.ragebot.weapons.scout.strict_hitchance);
    add(config.ragebot.weapons.scout.accuracy_boost);
    add(config.ragebot.weapons.scout.aim_head_if_safe);
    add(config.ragebot.weapons.deagle.hitboxes);
    add(config.ragebot.weapons.deagle.multipoints);
    add(config.ragebot.weapons.deagle.head_point_scale);
    add(config.ragebot.weapons.deagle.body_point_scale);
    add(config.ragebot.weapons.deagle.hitchance);
    add(config.ragebot.weapons.deagle.minimum_damage);
    add(config.ragebot.weapons.deagle.minimum_damage_override);
    add(config.ragebot.weapons.deagle.auto_stop);
    add(config.ragebot.weapons.deagle.auto_scope);
    add(config.ragebot.weapons.deagle.delay_shot);
    add(config.ragebot.weapons.deagle.strict_hitchance);
    add(config.ragebot.weapons.deagle.accuracy_boost);
    add(config.ragebot.weapons.deagle.aim_head_if_safe);
    add(config.ragebot.weapons.revolver.hitboxes);
    add(config.ragebot.weapons.revolver.multipoints);
    add(config.ragebot.weapons.revolver.head_point_scale);
    add(config.ragebot.weapons.revolver.body_point_scale);
    add(config.ragebot.weapons.revolver.hitchance);
    add(config.ragebot.weapons.revolver.minimum_damage);
    add(config.ragebot.weapons.revolver.minimum_damage_override);
    add(config.ragebot.weapons.revolver.auto_stop);
    add(config.ragebot.weapons.revolver.auto_scope);
    add(config.ragebot.weapons.revolver.delay_shot);
    add(config.ragebot.weapons.revolver.strict_hitchance);
    add(config.ragebot.weapons.revolver.accuracy_boost);
    add(config.ragebot.weapons.revolver.aim_head_if_safe);
    add(config.ragebot.weapons.pistol.hitboxes);
    add(config.ragebot.weapons.pistol.multipoints);
    add(config.ragebot.weapons.pistol.head_point_scale);
    add(config.ragebot.weapons.pistol.body_point_scale);
    add(config.ragebot.weapons.pistol.hitchance);
    add(config.ragebot.weapons.pistol.minimum_damage);
    add(config.ragebot.weapons.pistol.minimum_damage_override);
    add(config.ragebot.weapons.pistol.auto_stop);
    add(config.ragebot.weapons.pistol.auto_scope);
    add(config.ragebot.weapons.pistol.delay_shot);
    add(config.ragebot.weapons.pistol.strict_hitchance);
    add(config.ragebot.weapons.pistol.accuracy_boost);
    add(config.ragebot.weapons.pistol.aim_head_if_safe);
    add(config.antiaim.angles.pitch);
    add(config.antiaim.angles.yaw);
    add(config.antiaim.angles.yaw_jitter);
    add(config.antiaim.angles.modifier_value);
    add(config.antiaim.angles.manual_left);
    add(config.antiaim.angles.manual_right);
    add(config.antiaim.angles.manual_options);
    add(config.antiaim.angles.body_yaw);
    add(config.antiaim.angles.body_yaw_options);
    add(config.antiaim.angles.body_yaw_limit);
    add(config.antiaim.angles.inverter);
    add(config.antiaim.angles.legacy_desync);
    add(config.antiaim.fakelag.enabled);
    add(config.antiaim.fakelag.limit);
    add(config.antiaim.fakelag.variability);
    add(config.antiaim.misc.fake_duck);
    add(config.antiaim.misc.slow_walk);
    add(config.antiaim.misc.leg_movement);
    add(config.antiaim.misc.jitter_move);
    add(config.visuals.esp.enable);
    add(config.visuals.esp.dormant);
    add(config.visuals.esp.dormant_color);
    add(config.visuals.esp.bounding_box);
    add(config.visuals.esp.box_color);
    add(config.visuals.esp.health_bar);
    add(config.visuals.esp.custom_health);
    add(config.visuals.esp.custom_health_color);
    add(config.visuals.esp.name);
    add(config.visuals.esp.name_color);
    add(config.visuals.esp.flags);
    add(config.visuals.esp.weapon_text);
    add(config.visuals.esp.weapon_text_color);
    add(config.visuals.esp.weapon_icon);
    add(config.visuals.esp.weapon_icon_color);
    add(config.visuals.esp.ammo);
    add(config.visuals.esp.ammo_color);
    add(config.visuals.esp.glow);
    add(config.visuals.esp.glow_color);
    add(config.visuals.esp.hitsound);
    add(config.visuals.esp.shared_esp);
    add(config.visuals.esp.share_with_enemies);
    add(config.visuals.esp.hitmarker);
    add(config.visuals.esp.hitmarker_color);
    add(config.visuals.esp.damage_marker);
    add(config.visuals.esp.damage_marker_color);
    add(config.visuals.chams.enemy);
    add(config.visuals.chams.enemy_color);
    add(config.visuals.chams.enemy_second_color);
    add(config.visuals.chams.enemy_invisible);
    add(config.visuals.chams.enemy_invisible_color);
    add(config.visuals.chams.enemy_type);
    add(config.visuals.chams.enemy_glow_thickness);
    add(config.visuals.chams.shot_chams);
    add(config.visuals.chams.shot_chams_color);
    add(config.visuals.chams.shot_chams_type);
    add(config.visuals.chams.shot_chams_second_color);
    add(config.visuals.chams.shot_chams_thickness);
    add(config.visuals.chams.shot_chams_duration);
    add(config.visuals.chams.shot_chams_options);
    add(config.visuals.chams.local_player);
    add(config.visuals.chams.local_player_color);
    add(config.visuals.chams.local_player_type);
    add(config.visuals.chams.local_player_second_color);
    add(config.visuals.chams.local_glow_thickness);
    add(config.visuals.chams.attachments);
    add(config.visuals.chams.attachments_color);
    add(config.visuals.chams.attachments_type);
    add(config.visuals.chams.attachments_second_color);
    add(config.visuals.chams.attachments_glow_thickness);
    add(config.visuals.chams.viewmodel);
    add(config.visuals.chams.viewmodel_color);
    add(config.visuals.chams.viewmodel_type);
    add(config.visuals.chams.viewmodel_second_color);
    add(config.visuals.chams.viewmodel_glow_thickness);
    add(config.visuals.chams.disable_model_occlusion);
    add(config.visuals.other_esp.radar);
    add(config.visuals.other_esp.dropped_weapons);
    add(config.visuals.other_esp.dropped_weapons_color);
    add(config.visuals.other_esp.sniper_crosshair);
    add(config.visuals.other_esp.penetration_crosshair);
    add(config.visuals.other_esp.bomb);
    add(config.visuals.other_esp.bomb_color);
    add(config.visuals.other_esp.grenades);
    add(config.visuals.other_esp.grenade_trajecotry);
    add(config.visuals.other_esp.grenade_trajectory_color);
    add(config.visuals.other_esp.grenade_trajectory_hit_color);
    add(config.visuals.other_esp.grenade_proximity_warning);
    add(config.visuals.other_esp.grenade_predict_color);
    add(config.visuals.other_esp.particles);
    add(config.visuals.effects.fov);
    add(config.visuals.effects.fov_zoom);
    add(config.visuals.effects.fov_second_zoom);
    add(config.visuals.effects.removals);
    add(config.visuals.effects.remove_scope);
    add(config.visuals.effects.world_color_enable);
    add(config.visuals.effects.world_color);
    add(config.visuals.effects.props_color_enable);
    add(config.visuals.effects.props_color);
    add(config.visuals.effects.thirdperson);
    add(config.visuals.effects.thirdperson_bind);
    add(config.visuals.effects.thirdperson_distance);
    add(config.visuals.effects.aspect_ratio);
    add(config.visuals.effects.client_impacts);
    add(config.visuals.effects.client_impacts_color);
    add(config.visuals.effects.server_impacts);
    add(config.visuals.effects.server_impacts_color);
    add(config.visuals.effects.impacts_duration);
    add(config.visuals.effects.override_skybox);
    add(config.visuals.effects.override_fog);
    add(config.visuals.effects.fog_color);
    add(config.visuals.effects.fog_start);
    add(config.visuals.effects.fog_end);
    add(config.visuals.effects.fog_density);
    add(config.visuals.effects.preserve_killfeed);
    add(config.visuals.effects.custom_sun_direction);
    add(config.visuals.effects.sun_pitch);
    add(config.visuals.effects.sun_yaw);
    add(config.visuals.effects.sun_distance);
    add(config.visuals.effects.scope_blend);
    add(config.visuals.effects.viewmodel_scope_alpha);
    add(config.misc.miscellaneous.anti_untrusted);
    add(config.misc.miscellaneous.automatic_grenade_release);
    add(config.misc.miscellaneous.logs);
    add(config.misc.miscellaneous.auto_buy);
    add(config.misc.miscellaneous.filter_console);
    add(config.misc.miscellaneous.clantag);
    add(config.misc.miscellaneous.ad_block);
    add(config.misc.movement.auto_jump);
    add(config.misc.movement.auto_strafe);
    add(config.misc.movement.compensate_throwable);
    add(config.misc.movement.auto_strafe_smooth);
    add(config.misc.movement.edge_jump);
    add(config.misc.movement.edge_jump_key);
    add(config.misc.movement.infinity_duck);
    add(config.misc.movement.quick_stop);
    add(config.skins.override_knife);
    add(config.skins.override_agent);
    add(config.skins.knife_model);
    add(config.skins.agent_model_t);
    add(config.skins.agent_model_ct);
    add(config.skins.mask_changer);
    add(config.skins.mask_changer_models); 
    add(config.skins.paint_kits);

    load_button->SetCallback(on_load);
    save_button->SetCallback(on_save);
    config_list->SetCallback(on_config_list_changed);
}

void CConfig::parse(nlohmann::json& cfg) {
    for (auto& item : items) {
        try {
            IBaseWidget* e = item.item;
            auto& val = cfg[item.name];

            switch (item.item->GetType()) {
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

            for (auto cb : e->callbacks)
                cb();

            for (auto& lcb : e->lua_callbacks)
                lcb.func();
        }
        catch (nlohmann::json::exception& e) {
            Console->Error("item not found: " + item.name);
        }
    }

    try {
        for (std::string script_name : cfg["loaded_scripts"]) {
            int sid = Lua->GetScriptID(script_name);
            if (sid != -1)
                Lua->LoadScript(sid);
        }
    }
    catch (nlohmann::json::exception& e) {
        Console->Error("missing loaded scripts");
    }
}

nlohmann::json CConfig::dump() {
    nlohmann::json result;

    for (const auto& item : items) {
        IBaseWidget* e = item.item;
        std::string name = item.name;

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
            // handle any unexpected values of ElementType here
            break;
        }
    }

    result["loaded_scripts"] = Lua->GetLoadedScripts();

    return result;
}