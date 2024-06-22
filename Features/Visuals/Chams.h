#pragma once
#include "../../SDK/Misc/Color.h"
#include "../../SDK/Interfaces/IMaterialSystem.h"

#include <vector>

class IMaterial;
struct LagRecord;

struct ChamsMaterial {
	bool enabled = false;
	int type = 0;
	Color primaryColor;
	bool addZ = false;
	Color invisibleColor;
	Color secondaryColor;
	float glowThickness = 1;
};

struct ShotChams_t {
	int ent_index;
	ModelRenderInfo_t info;
	DrawModelState_t state;
	matrix3x4_t pBoneToWorld[128];
	float end_time;
	matrix3x4_t model_to_world;
};

class CChams {
	void* _ctx;
	DrawModelState_t _state;
	ModelRenderInfo_t _info;
	matrix3x4_t* _boneToWorld;

	IMaterial* baseMaterials[3];
	ChamsMaterial materials[5];

	std::vector<ShotChams_t> shot_chams;
public:
	void DrawModelExecute();

	void LoadChams();
	void UpdateSettings();
	bool OnDrawModelExecute(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pBoneToWorld);
	void DrawModel(ChamsMaterial& material, float alpha = 1.f, matrix3x4_t* customBoneToWorld = nullptr, bool ignorez = false);

	void AddShotChams(LagRecord* record);
	void RenderShotChams();
	void RemoveShotChams(int id);
};

extern CChams* Chams;