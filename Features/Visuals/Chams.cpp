#include "Chams.h"
#include "../../SDK/Hooks.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"
#include "../RageBot/LagCompensation.h"
#include "../../SDK/Misc/KeyValues.h"
#include "../../Utils/Console.h"
#include "../Lua/Bridge/Bridge.h"

#include <format>


enum MaterialType {
	Default,
	Flat,
	Glow,
	GlowOutline,
	GlowOverlay,
};

enum ClassOfEntity {
	Enemy,
	Shot,
	LocalPlayer,
	ViewModel,
	Attachment,
	TotalClasses
};

Vector interpolatedBacktrackChams[64];
CChams* Chams = new CChams;

void CChams::LoadChams() {
	static bool created = false;

	if (created)
		return;

	created = true;
	KeyValues* keyval_def = new KeyValues("VertexLitGeneric");
	keyval_def->SetString("$basetexture", "vgui/white");
	keyval_def->SetInt("$nofog", 1);
	baseMaterials[MaterialType::Default] = MaterialSystem->CreateMaterial("arctic/default", keyval_def);
	baseMaterials[MaterialType::Default]->IncrementReferenceCount();

	KeyValues* keyval_flat = new KeyValues("UnlitGeneric");
	keyval_flat->SetString("$basetexture", "vgui/white");
	keyval_flat->SetInt("$nofog", 1);
	baseMaterials[MaterialType::Flat] = MaterialSystem->CreateMaterial("arctic/flat", keyval_flat);
	baseMaterials[MaterialType::Flat]->IncrementReferenceCount();

	KeyValues* keyval_glow = new KeyValues("VertexLitGeneric");
	keyval_glow->SetInt("$additive", 1);
	keyval_glow->SetString("$envmap", "models/effects/cube_white");
	keyval_glow->SetInt("$envmapfresnel", 1);
	keyval_glow->SetString("$envmapfresnelminmaxexp", "[0 1 2]");
	keyval_glow->SetInt("$alpha", 1);
	keyval_glow->SetInt("$model", 1);
	keyval_glow->SetInt("$ignorez", 0);
	keyval_glow->SetInt("$translucnet", 0);
	keyval_glow->SetInt("$nofog", 1);
	baseMaterials[MaterialType::Glow] = MaterialSystem->CreateMaterial("arctic/glow", keyval_glow);
	baseMaterials[MaterialType::Glow]->IncrementReferenceCount();
}

void CChams::UpdateSettings() {
	materials[ClassOfEntity::Enemy].enabled = config.visuals.chams.enemy->get();
	materials[ClassOfEntity::Enemy].type = config.visuals.chams.enemy_type->get();
	materials[ClassOfEntity::Enemy].addZ = config.visuals.chams.enemy_invisible->get();
	materials[ClassOfEntity::Enemy].primaryColor = config.visuals.chams.enemy_color->get();
	materials[ClassOfEntity::Enemy].invisibleColor = config.visuals.chams.enemy_invisible_color->get();
	materials[ClassOfEntity::Enemy].secondaryColor = config.visuals.chams.enemy_second_color->get();
	materials[ClassOfEntity::Enemy].glowThickness = config.visuals.chams.enemy_glow_thickness->get();

	materials[ClassOfEntity::Shot].enabled = config.visuals.chams.shot_chams->get();
	materials[ClassOfEntity::Shot].type = config.visuals.chams.shot_chams_type->get();
	materials[ClassOfEntity::Shot].addZ = false;
	materials[ClassOfEntity::Shot].primaryColor = config.visuals.chams.shot_chams_color->get();
	materials[ClassOfEntity::Shot].secondaryColor = config.visuals.chams.shot_chams_second_color->get();
	materials[ClassOfEntity::Shot].glowThickness = config.visuals.chams.shot_chams_thickness->get();

	materials[ClassOfEntity::LocalPlayer].enabled = config.visuals.chams.local_player->get();
	materials[ClassOfEntity::LocalPlayer].type = config.visuals.chams.local_player_type->get();
	materials[ClassOfEntity::LocalPlayer].addZ = false;
	materials[ClassOfEntity::LocalPlayer].primaryColor = config.visuals.chams.local_player_color->get();
	materials[ClassOfEntity::LocalPlayer].secondaryColor = config.visuals.chams.local_player_second_color->get();
	materials[ClassOfEntity::LocalPlayer].glowThickness = config.visuals.chams.local_glow_thickness->get();

	materials[ClassOfEntity::ViewModel].enabled = config.visuals.chams.viewmodel->get();
	materials[ClassOfEntity::ViewModel].type = config.visuals.chams.viewmodel_type->get();
	materials[ClassOfEntity::ViewModel].addZ = false;
	materials[ClassOfEntity::ViewModel].primaryColor = config.visuals.chams.viewmodel_color->get();
	materials[ClassOfEntity::ViewModel].secondaryColor = config.visuals.chams.viewmodel_second_color->get();
	materials[ClassOfEntity::ViewModel].glowThickness = config.visuals.chams.viewmodel_glow_thickness->get();

	materials[ClassOfEntity::Attachment].enabled = config.visuals.chams.attachments->get();
	materials[ClassOfEntity::Attachment].type = config.visuals.chams.attachments_type->get();
	materials[ClassOfEntity::Attachment].addZ = false;
	materials[ClassOfEntity::Attachment].primaryColor = config.visuals.chams.attachments_color->get();
	materials[ClassOfEntity::Attachment].secondaryColor = config.visuals.chams.attachments_second_color->get();
	materials[ClassOfEntity::Attachment].glowThickness = config.visuals.chams.attachments_glow_thickness->get();
}

void CChams::DrawModelExecute() {
	static tDrawModelExecute drawModelExecute = (tDrawModelExecute)Hooks::ModelRenderVMT->GetOriginal(21);

	drawModelExecute(ModelRender, _ctx, _state, _info, _boneToWorld);
};

bool CChams::OnDrawModelExecute(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pBoneToWorld) {
	_ctx = ctx;
	_state = state;
	_info = info;
	_boneToWorld = pBoneToWorld;

	CBaseEntity* ent = EntityList->GetClientEntity(info.entity_index);
	std::string modelName = ModelInfoClient->GetModelName(info.pModel);

	bool isWeapon = modelName.starts_with("models/weapons/w");
	bool isArms = modelName.find("arms") != std::string::npos || modelName.find("sleeve") != std::string::npos;
	bool isViewModel = modelName.starts_with("models/weapons/v") && !isArms;

	ClassOfEntity entType;

	if (ent == Cheat.LocalPlayer) {
		entType = ClassOfEntity::LocalPlayer;
	}
	else if (ent && ent->IsPlayer()) {
		if (!ent->GetPlayer()->IsTeammate())
			entType = ClassOfEntity::Enemy;
		else
			return false;
	}
	else if (isViewModel) {
		entType = ClassOfEntity::ViewModel;
	}
	else if (isWeapon && (EntityList->GetClientEntityFromHandle(((CBaseCombatWeapon*)state.m_pRenderable->GetIClientUnknown())->moveparent()) == Cheat.LocalPlayer || 
		EntityList->GetClientEntityFromHandle(((CBaseCombatWeapon*)state.m_pRenderable->GetIClientUnknown())->m_hOwnerEntity()) == Cheat.LocalPlayer)) 
	{
		entType = ClassOfEntity::Attachment;
	}
	else if (isArms) {
		if (Cheat.LocalPlayer && Cheat.LocalPlayer->m_bIsScoped())
			RenderView->SetBlend(config.visuals.effects.viewmodel_scope_alpha->get() * 0.01f);
		return false;
	}
	else {
		return false;
	}

	for (auto& cb : Lua->hooks.getHooks(LUA_DRAW_CHAMS)) {
		auto result = cb.func((int)entType);

		if (result.valid()) {
			int code = result.get<int>();

			if (code == 0)
				return true;
			else if (code == 1)
				return false;
		}
	}

	ChamsMaterial& mat = materials[entType];

	float addAlpha = 1.f;

	if (Cheat.LocalPlayer && Cheat.LocalPlayer->m_bIsScoped()) {
		if (entType == Attachment || entType == LocalPlayer) {
			addAlpha = config.visuals.effects.scope_blend->get() * 0.01f;
		}
		else if (entType == ViewModel) {
			addAlpha = config.visuals.effects.viewmodel_scope_alpha->get() * 0.01f;
		}
	}

	if (entType == Attachment || entType == LocalPlayer) {
		for (auto& cb : Lua->hooks.getHooks(LUA_LOCAL_ALPHA)) {
			auto result = cb.func(Cheat.LocalPlayer, addAlpha);
			if (result.valid() && result.get_type() == sol::type::number)
				addAlpha = result.get<float>();
		}
	}

	if (!mat.enabled) {
		if (addAlpha == 1.f)
			return false;
		RenderView->SetBlend(addAlpha);
		return false;
	}

	int backup_type = mat.type;
	if (mat.type == MaterialType::GlowOverlay) {
		mat.type = MaterialType::GlowOutline;

		RenderView->SetBlend(addAlpha);
		DrawModelExecute();
	}

	DrawModel(mat, addAlpha);

	mat.type = backup_type;

	return true;
}

void CChams::DrawModel(ChamsMaterial& cham, float alpha, matrix3x4_t* customBoneToWorld, bool ignorez) {
	static auto drawModelExecute = (tDrawModelExecute)Hooks::ModelRenderVMT->GetOriginal(21);

	if (customBoneToWorld)
		_boneToWorld = customBoneToWorld;

	IMaterial* baseMat = baseMaterials[cham.type == MaterialType::Glow ? MaterialType::Default : cham.type];
	RenderView->SetColorModulation(1.f, 1.f, 1.f);

	if (cham.addZ) {
		if (cham.type != MaterialType::GlowOutline) {
			baseMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
			baseMat->ColorModulate(cham.invisibleColor);
			RenderView->SetBlend(cham.invisibleColor.a / 255.f * alpha);

			ModelRender->ForcedMaterialOverride(baseMat);
			DrawModelExecute();
			ModelRender->ForcedMaterialOverride(nullptr);
		}

		if (cham.type == MaterialType::Glow || cham.type == MaterialType::GlowOutline) {
			IMaterial* glowMat = baseMaterials[MaterialType::Glow];

			glowMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

			IMaterialVar* envmaptint = glowMat->FindVar("$envmaptint");
			IMaterialVar* envmapfranselminmax = glowMat->FindVar("$envmapfresnelminmaxexp");

			envmaptint->SetVecValue(cham.secondaryColor);
			envmapfranselminmax->SetVecValue(0, 1, 10.f - cham.glowThickness);

			RenderView->SetBlend(cham.secondaryColor.a / 255.f * alpha);

			ModelRender->ForcedMaterialOverride(glowMat);
			DrawModelExecute();
			ModelRender->ForcedMaterialOverride(nullptr);
		}
	}

	if (cham.type != MaterialType::GlowOutline) {
		baseMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
		baseMat->ColorModulate(cham.primaryColor);
		RenderView->SetBlend(cham.primaryColor.a / 255.f * alpha);

		ModelRender->ForcedMaterialOverride(baseMat);
		DrawModelExecute();
		ModelRender->ForcedMaterialOverride(nullptr);
	}

	if (cham.type == MaterialType::Glow || cham.type == MaterialType::GlowOutline) {
		IMaterial* glowMat = baseMaterials[MaterialType::Glow];

		glowMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);

		IMaterialVar* envmaptint = glowMat->FindVar("$envmaptint");
		IMaterialVar* envmapfranselminmax = glowMat->FindVar("$envmapfresnelminmaxexp");

		envmaptint->SetVecValue(cham.secondaryColor);
		envmapfranselminmax->SetVecValue(0, 1, 10.f - cham.glowThickness);

		RenderView->SetBlend(cham.secondaryColor.a / 255.f * alpha);

		ModelRender->ForcedMaterialOverride(glowMat);
		DrawModelExecute();
		ModelRender->ForcedMaterialOverride(nullptr);
	}

	RenderView->SetBlend(1.f);
}

void CChams::AddShotChams(LagRecord* record) {
	auto& hit = shot_chams.emplace_back();

	std::memcpy(hit.pBoneToWorld, record->clamped_matrix, 128 * sizeof(matrix3x4_t));
	hit.end_time = GlobalVars->realtime + config.visuals.chams.shot_chams_duration->get();

	hit.info.origin = record->m_vecOrigin;
	hit.info.angles = record->m_vecAbsAngles;

	auto renderable = record->player->GetClientRenderable();

	if (!renderable)
		return;

	auto model = record->player->GetModel();

	if (!model)
		return;

	auto hdr = *(studiohdr_t**)(record->player->GetStudioHdr());

	if (!hdr)
		return;

	hit.state.m_pStudioHdr = hdr;
	hit.state.m_pStudioHWData = MDLCache->GetHardwareData(model->studio);
	hit.state.m_pRenderable = renderable;
	hit.state.m_drawFlags = 0;

	hit.info.pRenderable = renderable;
	hit.info.pModel = model;
	hit.info.pLightingOffset = nullptr;
	hit.info.pLightingOrigin = nullptr;
	hit.info.hitboxset = record->player->m_nHitboxSet();
	hit.info.skin = record->player->m_nSkin();
	hit.info.body = record->player->m_nBody();
	hit.info.entity_index = record->player->EntIndex();
	hit.info.instance = MODEL_INSTANCE_INVALID;
	hit.info.flags = 0x1;

	hit.info.pModelToWorld = &hit.model_to_world;
	hit.state.m_pModelToWorld = &hit.model_to_world;

	hit.model_to_world.AngleMatrix(hit.info.angles, hit.info.origin);

	if (config.visuals.chams.shot_chams_options->get(1)) {
		for (auto it = shot_chams.begin(); it != shot_chams.end(); it++) {
			if (it->end_time != hit.end_time && GlobalVars->realtime < it->end_time) {
				it->end_time = GlobalVars->realtime;
			}
		}
	}
}

void CChams::RenderShotChams() {
	if (!Cheat.InGame) {
		shot_chams.clear();
		return;
	}

	auto ctx = MaterialSystem->GetRenderContext();

	if (!ctx)
		return;

	for (auto it = shot_chams.begin(); it != shot_chams.end();) {
		if (GlobalVars->realtime - it->end_time > 0.5f) {
			it = shot_chams.erase(it);
			continue;
		}

		CBaseEntity* ent = EntityList->GetClientEntity(it->ent_index);

		if (!ent) {
			it = shot_chams.erase(it);
			continue;
		}

		it->info.pRenderable = it->state.m_pRenderable = ent->GetClientRenderable();

		if (!it->state.m_pModelToWorld || !it->state.m_pRenderable || !it->state.m_pStudioHdr || !it->state.m_pStudioHWData ||
			!it->info.pRenderable || !it->info.pModelToWorld || !it->info.pModel) {
			++it;
			continue;
		}

		float alpha = 1.f - std::clamp((GlobalVars->realtime - it->end_time) * 2.f, 0.f, 1.f);

		_ctx = ctx;
		_info = it->info;
		_state = it->state;

		DrawModel(materials[ClassOfEntity::Shot], alpha, it->pBoneToWorld, config.visuals.chams.shot_chams_options->get(1));

		++it;
	}
}

void CChams::RemoveShotChams(int id) {
	for (auto it = shot_chams.begin(); it != shot_chams.end();) {
		if (it->ent_index == id) {
			it = shot_chams.erase(it);
			continue;
		}

		it++;
	}
}