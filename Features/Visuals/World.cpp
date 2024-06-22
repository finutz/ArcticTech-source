#include "World.h"

#include <string>
#include <unordered_map>

#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"
#include "../RageBot/AnimationSystem.h"
#include "../RageBot/AutoWall.h"
#include "../../Utils/Animation.h"

void CWorld::Modulation() {
	static std::unordered_map<std::string, Color> original_colors;

	if (!Cheat.InGame)
		return;

	for (auto i = MaterialSystem->FirstMaterial(); i != MaterialSystem->InvalidMaterial(); i = MaterialSystem->NextMaterial(i)) {
		IMaterial* material = MaterialSystem->GetMaterial(i);
			
		if (!material)
			continue;

		if (material->IsErrorMaterial())
			continue;

		if (strstr(material->GetTextureGroupName(), "World")) {
			if (config.visuals.effects.world_color_enable->get()) {
				float r, g, b;
				material->GetColorModulation(&r, &g, &b);

				auto it = original_colors.find(material->GetName());
				if (it == original_colors.end()) {
					original_colors.insert({ material->GetName(), Color().as_fraction(r, g, b) });
					it = original_colors.find(material->GetName());
				}

				const Color clr = config.visuals.effects.world_color->get();
				material->ColorModulate(it->second * clr);
				material->AlphaModulate(clr.a / 255.f);
			}
			else {
				auto it = original_colors.find(material->GetName());
				if (it != original_colors.end())
					material->ColorModulate(it->second);
				material->AlphaModulate(1);
			}
		}
		//else if (strstr(material->GetTextureGroupName(), "StaticProp")) {
		//	if (config.visuals.effects.props_color_enable->get()) {
		//		float r, g, b;
		//		material->GetColorModulation(&r, &g, &b);

		//		auto it = original_colors.find(material->GetName());
		//		if (it == original_colors.end()) {
		//			original_colors.insert({ material->GetName(), Color().as_fraction(r, g, b) });
		//			it = original_colors.find(material->GetName());
		//		}

		//		const Color clr = config.visuals.effects.props_color->get();
		//		material->ColorModulate(it->second * clr);
		//		material->AlphaModulate(clr.a / 255.f);
		//		material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
		//		cvars.r_DrawSpecificStaticProp->SetInt(0);
		//	}
		//	else {
		//		auto it = original_colors.find(material->GetName());
		//		if (it != original_colors.end())
		//			material->ColorModulate(it->second);
		//		material->AlphaModulate(1);
		//	}
		//}
	}

	for (int i = 0; i < StaticPropMgr->m_StaticProps.Count(); i++) {
		CStaticProp* prop = &StaticPropMgr->m_StaticProps[i];

		if (!prop)
			continue;

		if (config.visuals.effects.props_color_enable->get()) {
			Color clr = config.visuals.effects.props_color->get();
			prop->m_DiffuseModulation[0] = clr.r / 255.f;
			prop->m_DiffuseModulation[1] = clr.g / 255.f;
			prop->m_DiffuseModulation[2] = clr.b / 255.f;
			prop->m_DiffuseModulation[3] = 1.f;

			auto alpha_prop = prop->m_pClientAlphaProperty;
			if (alpha_prop)
				alpha_prop->SetAlphaModulation(clr.a);
		}
		else {
			prop->m_DiffuseModulation[0] = 1.f;
			prop->m_DiffuseModulation[1] = 1.f;
			prop->m_DiffuseModulation[2] = 1.f;
			prop->m_DiffuseModulation[3] = 1.f;

			auto alpha_prop = prop->m_pClientAlphaProperty;
			if (alpha_prop)
				alpha_prop->SetAlphaModulation(255);
		}
	}
}

void CWorld::Fog() {
	if (config.visuals.effects.override_fog->get()) {
		cvars.fog_override->SetInt(1);
		cvars.fog_start->SetInt(config.visuals.effects.fog_start->get() * 8);
		cvars.fog_end->SetInt(config.visuals.effects.fog_end->get() * 8);
		cvars.fog_maxdensity->SetFloat(config.visuals.effects.fog_density->get() * 0.01f);
		cvars.fog_color->SetString((std::to_string(config.visuals.effects.fog_color->get().r) + " " + std::to_string(config.visuals.effects.fog_color->get().g) + " " + std::to_string(config.visuals.effects.fog_color->get().b)).c_str());
	}
	else {
		cvars.fog_override->SetInt(0);
	}
}

void CWorld::DisablePostProcessing( )
{

}

void CWorld::SkyBox() {
	static auto load_skybox = reinterpret_cast<void(__fastcall*)(const char*)>(Utils::PatternScan("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));

	if (!Cheat.InGame)
		return;

	switch (config.visuals.effects.override_skybox->get()) {
	case 1:
		load_skybox("sky_csgo_cloudy01");
		break;
	case 2:
		load_skybox("sky_csgo_night02");
		break;
	case 3:
		load_skybox("sky_csgo_night02b");
		break;
	}
}

void CWorld::ProcessCamera(CViewSetup* view_setup) {
	if (!Cheat.InGame || !Cheat.LocalPlayer)
		return;

	if (config.visuals.effects.thirdperson->get() && Cheat.LocalPlayer->IsAlive()) {
		Input->m_fCameraInThirdPerson = true;
		QAngle angles; EngineClient->GetViewAngles(&angles);
		QAngle backAngle = QAngle(angles.yaw - 180, -angles.pitch, 0);
		backAngle.Normalize();
		Vector cameraDirection = Math::AngleVectors(angles);
		if (cameraDirection.z == 0.f) // fuck valve shitcode
			cameraDirection.z = 0.01f;

		CGameTrace trace;
		CTraceFilterWorldOnly filter;
		Ray_t ray;
		Vector eyePos = (ctx.fake_duck ? Vector(0, 0, 64) : Cheat.LocalPlayer->m_vecViewOffset()) + Cheat.LocalPlayer->GetAbsOrigin();
		ray.Init(eyePos, eyePos - cameraDirection * config.visuals.effects.thirdperson_distance->get(), Vector(-16, -16, -16), Vector(16, 16, 16));

		EngineTrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

		float distance = trace.fraction * config.visuals.effects.thirdperson_distance->get();

		Input->m_vecCameraOffset = Vector(angles.pitch, angles.yaw, distance);
	}
	else {
		Input->m_fCameraInThirdPerson = false;
	}

	if (Cheat.LocalPlayer && (!Cheat.LocalPlayer->IsAlive() || Cheat.LocalPlayer->m_iTeamNum() == 1) && Cheat.LocalPlayer->m_iObserverMode() == OBS_MODE_CHASE) { // disable spectators interpolation
		CBasePlayer* observer = (CBasePlayer*)EntityList->GetClientEntityFromHandle(Cheat.LocalPlayer->m_hObserverTarget());

		if (observer) {
			Vector dir = Math::AngleVectors(view_setup->angles);
			Vector origin = AnimationSystem->GetInterpolated(observer) + Vector(0, 0, 64 - observer->m_flDuckAmount() * 28);
			CGameTrace trace = EngineTrace->TraceHull(origin, 
				origin - dir * config.visuals.effects.thirdperson_distance->get(), Vector(-20, -20, -20), Vector(20, 20, 20), CONTENTS_SOLID, observer);
			view_setup->origin = trace.endpos;
		}
	}
}

void CWorld::Smoke() {
	if (!Cheat.InGame)
		return;

	static const char* smokeMaterials[] = {
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_smokegrenade"
	};

	for (const char* mat : smokeMaterials) {
		IMaterial* material = MaterialSystem->FindMaterial(mat);
		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.visuals.effects.removals->get(3));
	}
}

void CWorld::RemoveBlood() {
	if (!Cheat.InGame)
		return;

	static const char* bloodMaterials[] = {
		"decals/blood1_subrect.vmt",
		"decals/blood2_subrect.vmt",
		"decals/blood3_subrect.vmt",
		"decals/blood4_subrect.vmt",
		"decals/blood5_subrect.vmt",
		"decals/blood6_subrect.vmt",
		"decals/blood1.vmt",
		"decals/blood2.vmt",
		"decals/blood3.vmt",
		"decals/blood4.vmt",
		"decals/blood5.vmt",
		"decals/blood6.vmt",
		"decals/blood7.vmt",
		"decals/blood8.vmt",
	};

	for (const char* mat : bloodMaterials) {
		IMaterial* material = MaterialSystem->FindMaterial(mat);

		if (material && !material->IsErrorMaterial()) {
			material->IncrementReferenceCount();
			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.visuals.effects.removals->get(5));
		}
	}
}

void CWorld::Crosshair() {
	cvars.weapon_debug_spread_show->SetInt((config.visuals.other_esp.sniper_crosshair->get() && Cheat.LocalPlayer && Cheat.LocalPlayer->IsAlive() && !Cheat.LocalPlayer->m_bIsScoped()) ? 2 : 0);

	if (Cheat.InGame && Cheat.LocalPlayer->IsAlive() && config.visuals.effects.remove_scope->get() == 1 && Cheat.LocalPlayer->m_bIsScoped()) {
		Render->BoxFilled(Vector2(Cheat.ScreenSize.x / 2, 0), Vector2(Cheat.ScreenSize.x / 2 + 1, Cheat.ScreenSize.y), Color(10, 10, 10, 255));
		Render->BoxFilled(Vector2(0, Cheat.ScreenSize.y / 2), Vector2(Cheat.ScreenSize.x, Cheat.ScreenSize.y / 2 + 1), Color(10, 10, 10, 255));
	}

	if (!config.visuals.other_esp.penetration_crosshair->get())
		return;

	if (!Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive())
		return;

	CBaseCombatWeapon* activeWeapon = Cheat.LocalPlayer->GetActiveWeapon();

	if (!activeWeapon || !activeWeapon->ShootingWeapon())
		return;

	Vector eyePos = (ctx.fake_duck ? Vector(0, 0, 64) : Cheat.LocalPlayer->m_vecViewOffset()) + Cheat.LocalPlayer->GetAbsOrigin();

	QAngle viewAngle;
	EngineClient->GetViewAngles(&viewAngle);

	Vector direction = Math::AngleVectors(viewAngle);

	if (direction.z == 0.f)
		direction.z = 0.01f;

	FireBulletData_t fb_data;
	bool hit = AutoWall->FireBullet(Cheat.LocalPlayer, eyePos, eyePos + direction * 8192, fb_data);

	Color crosshairColor(255, 20, 20);

	if (fb_data.num_impacts > 1) {
		crosshairColor = Color(20, 255, 20);
	}

	if (hit)
		crosshairColor = Color(56, 209, 255);

	Render->BoxFilled(Cheat.ScreenSize * 0.5f - Vector2(0, 1), Cheat.ScreenSize * 0.5f + Vector2(1, 2), crosshairColor);
	Render->BoxFilled(Cheat.ScreenSize * 0.5f - Vector2(1, 0), Cheat.ScreenSize * 0.5f + Vector2(2, 1), crosshairColor);
}

void CWorld::SunDirection() {
	static bool backupShouldReset;
	static Vector backupDirection, backupEnvDirection;
	static float backupDistance = -1.f;

	if (!Cheat.InGame) {
		backupShouldReset = false;
		backupDistance = -1.f;
		return;
	}

	static float lerp_sun_pitch = 0;
	static float lerp_sun_yaw = 0;

	static auto cascade_light_ptr = *reinterpret_cast<CCascadeLight***>(Utils::PatternScan("client.dll", "A1 ? ? ? ? B9 ? ? ? ? 56 F3 0F 7E 80", 0x1));
	
	CCascadeLight* cascade_light_entity = *cascade_light_ptr;

	if (!cascade_light_entity) {
		backupShouldReset = false;
		backupDistance = -1.f;
		return;
	}

	if (config.visuals.effects.custom_sun_direction->get()) {
		if (backupDistance == -1.f) {
			backupDirection = cascade_light_entity->m_shadowDirection();
			backupEnvDirection = cascade_light_entity->m_envLightShadowDirection();
			backupDistance = cascade_light_entity->m_flMaxShadowDist();
		}

		lerp_sun_pitch = interpolate(lerp_sun_pitch, config.visuals.effects.sun_pitch->get(), GlobalVars->frametime * 8);
		lerp_sun_yaw = interpolate(lerp_sun_yaw, config.visuals.effects.sun_yaw->get(), GlobalVars->frametime * 8);

		Vector newDirection = Math::AngleVectors(QAngle(lerp_sun_pitch, lerp_sun_yaw, 0));

		cascade_light_entity->m_envLightShadowDirection() = newDirection;
		cascade_light_entity->m_shadowDirection() = newDirection;
		cascade_light_entity->m_flMaxShadowDist() = config.visuals.effects.sun_distance->get();
		backupShouldReset = true;
	}
	else if (backupShouldReset) { 
		cascade_light_entity->m_envLightShadowDirection() = backupEnvDirection;
		cascade_light_entity->m_shadowDirection() = backupDirection;
		cascade_light_entity->m_flMaxShadowDist() = backupDistance;
	}
}

CWorld* World = new CWorld;