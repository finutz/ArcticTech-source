#include "ESP.h"

#include <vector>
#include <string>
#include <algorithm>

#include "../../SDK/NetMessages.h"
#include "../../SDK/Globals.h"
#include "../../SDK/Misc/CBaseCombatWeapon.h"
#include "../../Utils/Utils.h"
#include "GrenadePrediction.h"
#include "../RageBot/LagCompensation.h"
#include "../RageBot/AnimationSystem.h"
#include "WeaponIcons.h"

ESPInfo_t ESPInfo[64];
GrenadeWarning NadeWarning;
CUtlVector<SndInfo_t> soundList;

struct ESPFlag_t {
	std::string flag;
	Color color;
};

float g_LastSharedESPData[64];

void ESP::Draw() {
	if (!config.visuals.esp.enable->get() || !Cheat.InGame || !Cheat.LocalPlayer) return;

	for (int i = 0; i < ClientState->m_nMaxClients; i++) {
		ESP::UpdatePlayer(i);
		ESP::DrawPlayer(i);
	}
}

void ESP::IconDisplay( CBasePlayer* player, int Level )
{
	static void* DT_CSPlayerResource = NULL;

	if ( DT_CSPlayerResource == NULL )
		DT_CSPlayerResource = Utils::PatternScan( "client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7", 0x2 );

	if ( !DT_CSPlayerResource )
		return;

	DWORD ptrResource = **( DWORD** )DT_CSPlayerResource;
	DWORD m_nPersonaDataPublicLevel = ( DWORD )ptrResource + 0x4dd4 + ( player->EntIndex( ) * 4 );

	*( PINT )( ( DWORD )m_nPersonaDataPublicLevel ) = Level;
}

void ESP::RegisterCallback() {
	NetMessages->AddArcticDataCallback(ProcessSharedESP);
}

void ESP::ProcessSharedESP(const SharedVoiceData_t* data) {
	SharedESP_t esp = *(SharedESP_t*)data;

	int ent_index = EngineClient->GetPlayerForUserID(esp.m_iPlayer);
	auto& esp_info = ESPInfo[ent_index];

	CBasePlayer* player = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntity(ent_index));

	if (!player || player == Cheat.LocalPlayer || !player->m_bDormant())
		return;

	//esp_info.m_vecOrigin = esp.m_vecOrigin;
	esp_info.m_iActiveWeapon = esp.m_ActiveWeapon;
	esp_info.m_flLastUpdateTime = GlobalVars->curtime;
	esp_info.m_nHealth = esp.m_iHealth;
	esp_info.m_bValid = true;

	if (player) {
		player->m_vecOrigin() = Vector(esp.m_vecOrigin.x, esp.m_vecOrigin.y, esp.m_vecOrigin.z);
		player->m_iHealth() = esp.m_iHealth;
	}

	g_LastSharedESPData[ent_index] = GlobalVars->realtime;
}

void ESP::ProcessSound(const SoundInfo_t& sound) {
	if (!config.visuals.esp.dormant->get())
		return;

	if (sound.nEntityIndex == 0 || sound.vOrigin.Zero())
		return;

	CBaseEntity* sound_source = EntityList->GetClientEntity(sound.nEntityIndex);

	if (!sound_source)
		return;

	CBasePlayer* player = nullptr;
	CBasePlayer* moveparent = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntityFromHandle(sound_source->moveparent()));
	CBasePlayer* owner = nullptr;
		
	if (sound_source->IsWeapon()) {
		owner = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntityFromHandle(reinterpret_cast<CBaseCombatWeapon*>(sound_source)->m_hOwner()));
		if (!owner)
			owner = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntityFromHandle(reinterpret_cast<CBaseCombatWeapon*>(sound_source)->m_hOwnerEntity()));
	}

	if (sound_source->IsPlayer())
		player = reinterpret_cast<CBasePlayer*>(sound_source);
	else if (moveparent && moveparent->IsPlayer())
		player = moveparent;
	else if (owner && owner->IsPlayer())
		player = owner;
	else
		return;

	if (!player || player == Cheat.LocalPlayer || !player->m_bDormant() || player->IsTeammate() || !player->IsAlive())
		return;

	if (abs(GlobalVars->realtime - g_LastSharedESPData[player->EntIndex()]) < 0.5f)
		return;

	Vector origin = sound.vOrigin;

	auto trace = EngineTrace->TraceHull(origin, origin - Vector(0, 0, 128), Vector(-16, -16, 0), Vector(16, 16, 72), MASK_SOLID, player);
	player->m_vecOrigin() = trace.fraction == 1.f ? trace.startpos : trace.endpos;

	ESPInfo_t& esp_info = ESPInfo[player->EntIndex()];

	esp_info.m_flLastUpdateTime = GlobalVars->curtime;
	esp_info.m_bValid = true;

	if (esp_info.m_nHealth <= 0)
		esp_info.m_nHealth = 100;
}

void ESP::UpdatePlayer(int id) {
	CBasePlayer* player = (CBasePlayer*)EntityList->GetClientEntity(id);
	ESPInfo_t& info = ESPInfo[id];

	info.m_pEnt = player;
	if (!player) {
		info.m_bValid = false;
		return;
	}

	if (player->m_iTeamNum() == 1) {
		info.m_bValid = false;
		info.m_nHealth = 0;

		if (player->m_iObserverMode() != OBS_MODE_IN_EYE || player->m_iObserverMode() != OBS_MODE_CHASE)
			return;

		CBasePlayer* spec_pl = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntityFromHandle(player->m_hObserverTarget()));
		if (!spec_pl)
			return;

		if (spec_pl->IsTeammate())
			return;

		ESPInfo_t& spec_info = ESPInfo[spec_pl->EntIndex()];
		spec_info.m_vecOrigin = spec_pl->m_vecOrigin();
		spec_info.m_flLastUpdateTime = GlobalVars->curtime;
		return;
	}

	if (player->IsTeammate() || !player->IsAlive()) {
		info.m_flLastUpdateTime = GlobalVars->curtime - 12.f;
		info.m_bValid = false;
		return;
	}

	info.m_bDormant = player->m_bDormant();

	auto& records = LagCompensation->records(id);

	if (records.empty()) {
		info.m_bDormant = true;
	}

	if (!info.m_bDormant)
		info.m_nHealth = player->m_iHealth();

	if (!info.m_bDormant) {
		LagRecord* latestRecord = &records.back();

		if (latestRecord->m_flDuckAmout > 0.01f && latestRecord->m_flDuckAmout < 0.9 && player->m_flDuckSpeed() == 8)
			info.m_nFakeDuckTicks++;
		else
			info.m_nFakeDuckTicks = 0;

		info.m_vecOrigin = AnimationSystem->GetInterpolated(player);
		info.m_bFakeDuck = info.m_nFakeDuckTicks > 14;
		info.m_bExploiting = latestRecord->shifting_tickbase;
		info.m_bBreakingLagComp = latestRecord->breaking_lag_comp;
	}
	else {
		if (info.m_nHealth == 0) {
			info.m_bValid = false;
		}

		info.m_nFakeDuckTicks = 0;
		info.m_bFakeDuck = false;
		info.m_bExploiting = false;
		info.m_bBreakingLagComp = false;

		info.m_vecOrigin.Interpolate(player->m_vecOrigin(), GlobalVars->frametime * 32);
	}

	float playerHeight = player->m_vecMaxs().z;
	if (playerHeight == 0) {
		info.m_bValid = false;
		return;
	}

	Vector2 head = Render->WorldToScreen(info.m_vecOrigin + Vector(0, 0, playerHeight));
	Vector2 feet = Render->WorldToScreen(info.m_vecOrigin);
	int h = feet.y - head.y;
	int w = (h / playerHeight) * 32;
	int bboxCenter = (feet.x + head.x) * 0.5f;
	info.m_BoundingBox[0] = Vector2(bboxCenter - w * 0.5f, head.y);
	info.m_BoundingBox[1] = Vector2(bboxCenter + w * 0.5f, feet.y);

	info.m_bValid = info.m_BoundingBox[0].x > 0 && info.m_BoundingBox[0].y > 0 && info.m_BoundingBox[1].x < Cheat.ScreenSize.x&& info.m_BoundingBox[1].y < Cheat.ScreenSize.y;

	if (info.m_bDormant) {
		float unupdatedTime = GlobalVars->curtime - info.m_flLastUpdateTime;

		if (unupdatedTime > 5)
			info.m_flAlpha = (10 - unupdatedTime) * 0.2f;
		else
			info.m_flAlpha = 1.f;

		if (unupdatedTime > 10)
			info.m_bValid = false;
	}
	else {
		info.m_flAlpha = 1.f;
		info.m_flLastUpdateTime = GlobalVars->curtime;
		
		CBaseCombatWeapon* weapon = player->GetActiveWeapon();

		if (weapon)
			info.m_iActiveWeapon = weapon->m_iItemDefinitionIndex();
	}
}

void ESP::DrawPlayer(int id) {
	ESPInfo_t info = ESPInfo[id];

	if (!info.m_bValid)
		return;

	CBasePlayer* player = info.m_pEnt;

	if (info.m_bDormant && !config.visuals.esp.dormant->get())
		return;

	if (info.m_BoundingBox[0].Invalid() || info.m_BoundingBox[1].Invalid())
		return;

	DrawBox(info);
	DrawHealth(info);
	DrawName(info);
	DrawFlags(info);
	DrawWeapon(info);
}

void ESP::DrawBox(ESPInfo_t info) {
	if (!config.visuals.esp.bounding_box->get())
		return;

	Color clr = config.visuals.esp.box_color->get();
	if (info.m_bDormant) {
		clr = config.visuals.esp.dormant_color->get();
	}
	clr.a *= info.m_flAlpha;

	Render->Box(info.m_BoundingBox[0], info.m_BoundingBox[1], clr);
	Render->Box(info.m_BoundingBox[0] - Vector2(1, 1), info.m_BoundingBox[1] + Vector2(1, 1), Color(0, 0, 0, 150 * clr.a / 255));
	Render->Box(info.m_BoundingBox[0] + Vector2(1, 1), info.m_BoundingBox[1] - Vector2(1, 1), Color(0, 0, 0, 150 * clr.a / 255));
}

void ESP::DrawHealth(ESPInfo_t info) {
	if (!config.visuals.esp.health_bar->get())
		return;

	Color clr = Color(0, 255, 0);

	int health = info.m_nHealth;
	clr.r = std::clamp(health < 50 ? 250 : (100 - health) / 50.f * 250.f, 120.f, 230.f);
	clr.g = std::clamp(health > 50 ? 250 : health / 50.f * 250.f, 120.f, 230.f);
	clr.b = 120;

	if (config.visuals.esp.custom_health->get())
		clr = config.visuals.esp.custom_health_color->get();

	if (info.m_bDormant)
		clr = config.visuals.esp.dormant_color->get().alpha_modulatef(info.m_flAlpha);

	float clr_a = clr.a / 255.f;

	int h = info.m_BoundingBox[1].y - info.m_BoundingBox[0].y;
	float health_fraction = std::clamp(1 - health / 100.f, 0.f, 1.f);
	Vector2 health_box_start = info.m_BoundingBox[0] - Vector2(7, 1);
	Vector2 health_box_end(info.m_BoundingBox[0].x - 3, info.m_BoundingBox[1].y + 1);

	Render->BoxFilled(health_box_start, health_box_end, Color(16, 16, 16, 220 * info.m_flAlpha * clr_a));

	Render->BoxFilled(health_box_start + Vector2(0, 1), Vector2(health_box_start.x + 1, health_box_end.y - 1), Color(10, 10, 10, 245 * info.m_flAlpha * clr_a));
	Render->BoxFilled(Vector2(health_box_end.x - 1, health_box_start.y + 1), Vector2(health_box_end.x, health_box_end.y - 1), Color(10, 10, 10, 245 * info.m_flAlpha * clr_a));
	Render->BoxFilled(health_box_start, Vector2(health_box_end.x, health_box_start.y + 1), Color(10, 10, 10, 245 * info.m_flAlpha * clr_a));
	Render->BoxFilled(Vector2(health_box_start.x, health_box_end.y - 1), health_box_end, Color(10, 10, 10, 245 * info.m_flAlpha * clr_a));

	Render->BoxFilled(health_box_start + Vector2(1, 1 + h * health_fraction), health_box_end - Vector2(1, 1), clr);

	if (health < 92)
		Render->Text(std::to_string(health), info.m_BoundingBox[0] - Vector2(7.f, -health_fraction * h + 5), Color(240, 240, 240, 255 * info.m_flAlpha * clr_a), SmallFont, TEXT_CENTERED | TEXT_OUTLINED);
}

void ESP::DrawName(ESPInfo_t info) {
	if (!config.visuals.esp.name->get())
		return;

	Color clr = config.visuals.esp.name_color->get();
	if (info.m_bDormant) {
		clr = config.visuals.esp.dormant_color->get();
	}

	clr.a *= info.m_flAlpha;

	Render->Text(info.m_pEnt->GetName(), Vector2((info.m_BoundingBox[0].x + info.m_BoundingBox[1].x) * 0.5f, info.m_BoundingBox[0].y - 13), clr, Verdana, TEXT_CENTERED | TEXT_DROPSHADOW);
}

void ESP::DrawFlags(ESPInfo_t info) {
	bool dormant = info.m_bDormant;

	auto& records = LagCompensation->records(info.m_pEnt->EntIndex());

	LagRecord* record = nullptr;
	if (!records.empty())
		record = &records.back();

	std::vector<ESPFlag_t> flags;

	if (config.visuals.esp.flags->get(0) && !dormant) {
		std::string str = "";
		if (info.m_pEnt->m_ArmorValue() > 0)
			str = "K";
		if (info.m_pEnt->m_bHasHelmet())
			str = "HK";
		if (!str.empty())
			flags.push_back({ str, Color(240, 240, 240, info.m_flAlpha) });
	}

	bool shifting = record && record->shifting_tickbase;

	if (config.visuals.esp.flags->get(4) && info.m_pEnt->EntIndex() == PlayerResource->m_iPlayerC4())
		flags.push_back({ "BOMB", Color(230, 80, 80, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(1) && info.m_pEnt->m_bIsScoped() && !dormant)
		flags.push_back({ "ZOOM", Color(120, 160, 200, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(3) && record && (shifting || record->exploiting) && !dormant)
		flags.push_back({ "X", shifting ? Color(230, 60, 60, 255 * info.m_flAlpha) : Color(215, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(2) && info.m_bFakeDuck && !dormant)
		flags.push_back({ "FD", Color(215, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(5) && info.m_bBreakingLagComp && !dormant)
		flags.push_back({ "LC", Color(230, 80, 80, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(6) && record && record->resolver_data.antiaim_type == R_AntiAimType::JITTER && !dormant)
		flags.push_back({ "J", Color(215, 255 * info.m_flAlpha) });

	if (config.visuals.esp.flags->get(6) && record && record->resolver_data.resolver_type == ResolverType::ANIM && !dormant)
		flags.push_back({ "ANIM", Color(165, 230, 14, 255 * info.m_flAlpha) });

	int line_offset = 0;
	const Color dormant_color = config.visuals.esp.dormant_color->get().alpha_modulatef(info.m_flAlpha);
	for (const auto& flag : flags) {
		Render->Text(flag.flag, Vector2(info.m_BoundingBox[1].x + 3, info.m_BoundingBox[0].y + line_offset), dormant ? dormant_color : flag.color, SmallFont, TEXT_OUTLINED);
		line_offset += 10;
	}
}

void ESP::DrawWeapon(ESPInfo_t info) {
	if (!config.visuals.esp.weapon_text->get() && !config.visuals.esp.weapon_icon->get())
		return;

	if (!info.m_iActiveWeapon)
		return;

	auto weapon_data = WeaponSystem->GetWeaponData(info.m_iActiveWeapon);

	if (!weapon_data)
		return;

	std::string weap = info.m_pEnt->GetActiveWeapon()->GetName(weapon_data);
	int current_offset = 0;
	if (config.visuals.esp.weapon_text->get()) {
		Render->Text(weap, Vector2((info.m_BoundingBox[0].x + info.m_BoundingBox[1].x) / 2, info.m_BoundingBox[1].y + 2), info.m_bDormant ? config.visuals.esp.dormant_color->get().alpha_modulatef(info.m_flAlpha) : config.visuals.esp.weapon_text_color->get().alpha_modulatef(info.m_flAlpha), SmallFont, TEXT_OUTLINED | TEXT_CENTERED);
		current_offset += 11;
	}

	if (config.visuals.esp.weapon_icon->get()) {
		auto& wicon = WeaponIcons->GetIcon(info.m_iActiveWeapon);

		if (wicon.texture) {
			Render->Image(wicon, Vector2((info.m_BoundingBox[0].x + info.m_BoundingBox[1].x) / 2 - wicon.width * 0.5f + 1, info.m_BoundingBox[1].y + 2 + current_offset + 1), Color(8, (int)(info.m_flAlpha * 125)));
			Render->Image(wicon, Vector2((info.m_BoundingBox[0].x + info.m_BoundingBox[1].x) / 2 - wicon.width * 0.5f, info.m_BoundingBox[1].y + 2 + current_offset), info.m_bDormant ? config.visuals.esp.dormant_color->get().alpha_modulatef(info.m_flAlpha) : config.visuals.esp.weapon_icon_color->get().alpha_modulatef(info.m_flAlpha));
		}
	}
}

void ESP::DrawGrenades() {
	static auto mp_friendlyfire = CVar->FindVar("mp_friendlyfire");

	if (!Cheat.InGame || !config.visuals.other_esp.grenades->get()) return;
	
	for (int i = 0; i < EntityList->GetMaxEntities(); i++) {
		CBaseGrenade* ent = reinterpret_cast<CBaseGrenade*>(EntityList->GetClientEntity(i));

		if (!ent)
			continue;

		auto classId = ent->GetClientClass();

		if (!classId)
			continue;

		if (classId->m_ClassID == C_INFERNO) {
			CBasePlayer* owner = reinterpret_cast<CBasePlayer*>(EntityList->GetClientEntityFromHandle(ent->m_hOwnerEntity()));

			if (owner != Cheat.LocalPlayer && owner->IsTeammate() && !mp_friendlyfire->GetInt())
				continue;

			Vector2 pos = Render->WorldToScreen(ent->GetAbsOrigin());

			if (pos.Invalid())
				continue;

			float distance = (Cheat.LocalPlayer->GetAbsOrigin() - ent->GetAbsOrigin()).Q_Length();

			if (distance > 700)
				continue;

			float distance_alpha = std::clamp(1.f - (distance - 600.f) / 100.f, 0.f, 1.f);

			float alpha = std::clamp((7.03125f - (GlobalVars->curtime - ent->m_flInfernoSpawnTime())) * 2.f, 0.f, 1.f) * distance_alpha;

			Render->CircleFilled(pos, 30, Color(16, 16, 16, 190 * alpha));
			Render->GlowCircle2(pos, 27, Color(40, 40, 40, 255 * alpha), Color(20, 20, 20, 255 * alpha));
			Render->GlowCircle(pos, 25, Color(255, 50, 50, std::clamp((430 - distance) / 250.f, 0.f, 1.f) * 255 * alpha));

			Render->Image(Resources::Inferno, pos - Vector2(15, 15), Color(255, 255, 255, 230 * alpha));

			continue;
		}

		const model_t* model = ent->GetModel();

		if (!model)
			continue;

		studiohdr_t* studioModel = ModelInfoClient->GetStudioModel(model);

		if (!studioModel)
			continue;

		if (strstr(studioModel->szName, "thrown") || classId->m_ClassID == C_BASE_CS_GRENADE_PROJECTILE || classId->m_ClassID == C_MOLOTOV_PROJECTILE || classId->m_ClassID == C_DECOY_PROJECTILE) {
			int weapId;

			CBasePlayer* thrower = ent->GetThrower();

			if (thrower && thrower->IsTeammate() && mp_friendlyfire->GetInt() == 0 && thrower != Cheat.LocalPlayer)
				continue;

			if (strstr(studioModel->szName, "fraggrenade"))
			{
				if (ent->GetCreationTime() + 1.625f <= TICKS_TO_TIME(GlobalVars->tickcount))
					continue;

				weapId = HeGrenade;
			}
			else if (strstr(studioModel->szName, "incendiarygrenade") || strstr(studioModel->szName, "molotov"))
			{
				weapId = Molotov;
			}
			else
				continue;

			if (config.visuals.other_esp.grenade_proximity_warning->get()) {
				NadeWarning.Warning(ent, weapId);
			}
		}
	}
}

struct DamageMarker_t {
	Vector position;
	float time = 0.f;
	int damage = 0;
};

struct Hitmarker_t {
	Vector position;
	float time = 0.f;
};

static std::vector<DamageMarker_t> s_DamageMarkers;
static std::vector<Hitmarker_t> s_Hitmarkers;

void ESP::AddHitmarker(const Vector& position) {
	s_Hitmarkers.emplace_back(Hitmarker_t{ position, GlobalVars->curtime });
}

void ESP::AddDamageMarker(const Vector& position, int damage) {
	s_DamageMarkers.emplace_back(DamageMarker_t{ position, GlobalVars->curtime, damage });
}

void ESP::RenderMarkers() {
	if (!Cheat.InGame)
		return;

	for (auto it = s_DamageMarkers.begin(); it != s_DamageMarkers.end();) {
		if (GlobalVars->curtime - it->time > 1.5f) {
			it = s_DamageMarkers.erase(it);
			continue;
		}

		float timer = GlobalVars->curtime - it->time;
		float alpha = std::clamp((1.5f - timer) * 2.f, 0.f, 1.f);

		Vector world_pos = it->position + Vector(0, 0, timer * 30.f);

		Render->Text(std::to_string(it->damage), Render->WorldToScreen(world_pos), config.visuals.esp.damage_marker_color->get().alpha_modulatef(alpha), Verdana, TEXT_CENTERED | TEXT_DROPSHADOW);

		it++;
	}

	for (auto it = s_Hitmarkers.begin(); it != s_Hitmarkers.end();) {
		if (GlobalVars->curtime - it->time > 3.f) {
			it = s_Hitmarkers.erase(it);
			continue;
		}

		float timer = GlobalVars->curtime - it->time;
		float alpha = std::clamp(3.f - timer, 0.f, 1.f);

		Vector2 pos = Render->WorldToScreen(it->position);

		Render->Line(pos + Vector2(2, 2), pos + Vector2(5, 5), config.visuals.esp.hitmarker_color->get().alpha_modulatef(alpha));
		Render->Line(pos - Vector2(2, 2), pos - Vector2(5, 5), config.visuals.esp.hitmarker_color->get().alpha_modulatef(alpha));
		Render->Line(pos + Vector2(-2, 2), pos + Vector2(-5, 5), config.visuals.esp.hitmarker_color->get().alpha_modulatef(alpha));
		Render->Line(pos + Vector2(2, -2), pos + Vector2(5, -5), config.visuals.esp.hitmarker_color->get().alpha_modulatef(alpha));

		it++;
	}
}