#pragma once
#include "Misc/CBasePlayer.h"
#include "Config.h"

class ConVar;

struct CheatState_t {
	CBasePlayer* LocalPlayer = nullptr;
	bool InGame = false;
	Vector2 ScreenSize;
	bool Unloaded = false;
};

struct HooksInfo_t {
	bool update_csa = false;
	bool setup_bones = false;
	bool console_log = false;
	bool disable_interpolation = false;
	bool in_draw_static_props = false;
	bool disable_clamp_bones = false;
	bool read_packets = false;
};

struct Ctx_t {
	CUserCmd* cmd = nullptr;
	bool send_packet = true;
	bool is_peeking = false;
	int tickbase_shift = 0;
	int shifted_last_tick = 0;
	bool teleported_last_tick = false;
	Vector local_velocity;
	Vector last_local_velocity;
	bool update_nightmode = false;
	bool update_remove_blood = false;
	Vector local_sent_origin;
	bool breaking_lag_compensation = false;
	bool should_buy = false;
	QAngle leg_slide_angle;
	float last_shot_time = 0.f;
	bool planting_bomb = false;
	bool was_unregistered_shot = true;
	bool should_release_grenade = false;

	QAngle sent_angles;
	QAngle shot_angles;
	bool force_shot_angle = false;
	QAngle setup_bones_angle;

	CBaseCombatWeapon* active_weapon = nullptr;
	CCSWeaponData* weapon_info = nullptr;
	Vector shoot_position;
	bool fake_duck = false;
	int grenade_throw_tick = 0;

	int corrected_tickbase = 0;

	int lc_exploit = 0; // for tickbase correction
	int lc_exploit_prev = 0;
	int lc_exploit_change = 0;
	int lc_exploit_diff = 0;

	std::vector<int> shifted_commands;
	std::vector<int> sented_commands;

	void reset() {
		shifted_commands.clear();
		sented_commands.clear();
		tickbase_shift = 0;
		local_velocity = Vector();
		last_local_velocity = Vector();
		lc_exploit = lc_exploit_prev = false;
		breaking_lag_compensation = false;
		active_weapon = nullptr;
		weapon_info = nullptr;
		grenade_throw_tick = 0;
		fake_duck = 0;
		last_shot_time = 0.f;
		planting_bomb = false;
	}
};

struct CVars {
	ConVar* r_aspectratio;
	ConVar* mat_postprocessing_enable;
	ConVar* r_DrawSpecificStaticProp;
	ConVar* fog_override;
	ConVar* fog_color;
	ConVar* post_processing;
	ConVar* fog_start;
	ConVar* fog_end;
	ConVar* fog_maxdensity;
	ConVar* molotov_throw_detonate_time;
	ConVar* sv_cheats;
	ConVar* sv_gravity;
	ConVar* sv_jump_impulse;
	ConVar* sv_maxunlag;
	ConVar* weapon_recoil_scale;
	ConVar* cl_csm_shadows;
	ConVar* cl_foot_contact_shadows;
	ConVar* cl_lagcompensation;
	ConVar* cl_interp_ratio;
	ConVar* weapon_debug_spread_show;
	ConVar* r_drawsprites;
	ConVar* zoom_sensitivity_ratio_mouse;
	ConVar* mp_damage_headshot_only;
};

extern CheatState_t Cheat;
extern CVars cvars;
extern Ctx_t ctx;
extern HooksInfo_t hook_info;