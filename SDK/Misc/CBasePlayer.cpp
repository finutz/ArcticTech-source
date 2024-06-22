#include "CBasePlayer.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"

#include "../../Features/RageBot/AnimationSystem.h"

bool CBasePlayer::IsTeammate() {
	if (!Cheat.LocalPlayer || !this)
		return false;
	return Cheat.LocalPlayer->m_iTeamNum() == m_iTeamNum();
}

bool CBasePlayer::IsEnemy() {
	if (!Cheat.LocalPlayer)
		return true;

	return Cheat.LocalPlayer->m_iTeamNum() != m_iTeamNum();
}

Vector CBasePlayer::GetEyePosition() {
	if (this != Cheat.LocalPlayer) {
		return m_vecOrigin() + Vector(0, 0, 64 - (64 - 28) * m_flDuckAmount());
	}
	return m_vecOrigin() + m_vecViewOffset();
}

int CBasePlayer::GetButtonForced() {
	static const uint32_t m_afButtonsForced = *reinterpret_cast<uint32_t*>(Utils::PatternScan("client.dll", "8B 86 ? ? ? ? 09 47 30", 0x2));
	return *(int*)(this + m_afButtonsForced);
}

int CBasePlayer::GetButtonDisabled() {
	static const uint32_t m_afButtonsDisabled = *reinterpret_cast<uint32_t*>(Utils::PatternScan("client.dll", "8B 86 ? ? ? ? F7 D0 21 47 30", 0x2));
	return *(int*)(this + m_afButtonsDisabled);
}

CUserCmd** CBasePlayer::GetCurrentCommand() {
	static const uint32_t m_pCurrentCommand = *reinterpret_cast<uint32_t*>(Utils::PatternScan("client.dll", "89 BE ? ? ? ? E8 ? ? ? ? 85 FF", 0x2));
	return (CUserCmd**)((uint32_t)this + m_pCurrentCommand);
}

CUserCmd& CBasePlayer::GetLastCommand() {
	static const uint32_t m_LastCmd = *reinterpret_cast<uint32_t*>(Utils::PatternScan("client.dll", "8D 8E ? ? ? ? 89 5C 24 3C", 0x2));
	return *(CUserCmd*)((uint32_t)this + m_LastCmd);
}

void CBasePlayer::Think() {
	CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 139)(this);
}

bool CBasePlayer::PhysicsRunThink(int nThinkMetod) {
	static auto __physicsRunThink = (bool(__thiscall*)(CBasePlayer*, int))Utils::PatternScan("client.dll", "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87");
	return __physicsRunThink(this, nThinkMetod);
}

void CBasePlayer::PostThink() {
	CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 320)(this);
}

bool& CBasePlayer::m_bMaintainSequenceTransitions() {
	static auto _m_bMaintainSequenceTransitions = *reinterpret_cast<uintptr_t*>(Utils::PatternScan("client.dll", "80 BF ? ? ? ? ? 0F 84 ? ? ? ? 8B 43 08 8B 00 83 38 00", 0x2));
	return *(bool*)(this + _m_bMaintainSequenceTransitions);
}

void CBasePlayer::SetModelIndex( int modelIndex )
{
	return CallVFunction<void( __thiscall* )( void*, int )>( this, 75 )( this, modelIndex );
}

void CBasePlayer::SelectItem(const char* string, int subtype) {
	static auto select_item_fn = reinterpret_cast<void(__thiscall*)(void*, const char*, int)>(Utils::PatternScan("client.dll", "55 8B EC 56 8B F1 ? ? ? 85 C9 74 71 8B 06"));
	select_item_fn(this, string, subtype);
}

bool CBasePlayer::UsingStandardWeaponsInVehicle() {
	static auto fn = reinterpret_cast<bool(__thiscall*)(CBasePlayer*)>(Utils::PatternScan("client.dll", "56 57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 43"));
	return fn(this);
}

void CBasePlayer::UpdateButtonState(int buttons) {
	m_afButtonsLast() = *m_nButtons();

	*m_nButtons() = buttons;
	int buttonsChanged = m_afButtonsLast() ^ buttons;

	m_afButtonsPressed() = buttonsChanged & buttons;
	m_afButtonsReleased() = buttonsChanged & (~buttons);
}

int& CBasePlayer::m_nImpulse() {
	static uint32_t _m_nImpulse = *reinterpret_cast<uint32_t*>(Utils::PatternScan("client.dll", "89 86 ? ? ? ? 8B 57 30", 0x2));
	return *(int*)(this + _m_nImpulse);
}

CUtlVector<matrix3x4_t> CBasePlayer::GetCachedBoneData() {
	static auto cachedBoneData = *(DWORD*)(Utils::PatternScan("client.dll", "FF B7 ?? ?? ?? ?? 52", 0x2)) + 0x4;
	return *(CUtlVector<matrix3x4_t>*)(this + cachedBoneData);
}

float CBasePlayer::ScaleDamage(int hitgroup, CCSWeaponData* weaponData, float& damage) {
	if (hitgroup != HITGROUP_HEAD && cvars.mp_damage_headshot_only->GetInt() > 0) {
		damage = -1.f;
		return -1.f;
	}

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		damage *= 4.f;
		break;
	case HITGROUP_STOMACH:
		damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		damage *= 0.75f;
		break;
	default:
		break;
	}

	if (IsArmored(hitgroup))
	{
		int armorValue = m_ArmorValue();
		float armorRatio = weaponData->flArmorRatio * 0.5f;

		auto newDamage = damage * armorRatio;

		if (((damage - newDamage) * 0.5f) > armorValue)
		{
			newDamage = damage - (armorValue * 2.f);
		}

		damage = newDamage;
	}

	return damage;
}

Vector CBasePlayer::GetBonePosition(int bone) {
	matrix3x4_t boneMatrix = GetCachedBoneData()[bone];

	return Vector(boneMatrix[0][3], boneMatrix[1][3], boneMatrix[2][3]);
}

Vector CBasePlayer::GetHitboxCenter(int iHitbox, matrix3x4_t* pmatrix) {
	matrix3x4_t* matrix;

	if (!pmatrix) {
		matrix = GetCachedBoneData().Base();
	}
	else {
		matrix = pmatrix;
	}

	auto studio_model = ModelInfoClient->GetStudioModel(GetModel());
	if (studio_model) {
		auto hitbox = studio_model->GetHitboxSet(m_nHitboxSet())->GetHitbox(iHitbox);

		if (hitbox) {
			Vector min, max;

			Utils::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], min);
			Utils::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], max);

			return (min + max) / 2.0f;
		}
	}
	return Vector();
}

void CBasePlayer::UpdateClientSideAnimation() {
	bool backupClientAnim = m_bClientSideAnimation();

	m_bClientSideAnimation() = true;
	hook_info.update_csa = true;
	CallVFunction<void(__thiscall*)(CBasePlayer*)>(this, 224)(this);
	hook_info.update_csa = false;

	if (this != Cheat.LocalPlayer)
		m_bClientSideAnimation() = false;
}

void CBasePlayer::UpdateAnimationState(CCSGOPlayerAnimationState* state, const QAngle& angles, bool bForce) {
	static auto fn = Utils::PatternScan("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	// xmm2 eye yaw
	// xmm1 eye pitch
	// bForce on stack
	// ecx animstate

	//fn(state, nullptr, 0.0f, angles.yaw, angles.pitch, nullptr);

	float eyeYaw = angles.yaw;
	float eyePitch = angles.pitch;

	__asm {
		movss xmm2, eyePitch
		movss xmm1, eyeYaw
		mov ecx, state
		push bForce
		call fn
	}
}

AnimationLayer* CBasePlayer::GetAnimlayers()
{
	static int uAnimationOverlaysOffset = *reinterpret_cast<int*>(Utils::PatternScan("client.dll", "8B 89 ? ? ? ? 8D 0C D1", 0x2));
	return *(AnimationLayer**)(this + uAnimationOverlaysOffset);
}

CBaseCombatWeapon* CBasePlayer::GetActiveWeapon() {
	return (CBaseCombatWeapon*)EntityList->GetClientEntityFromHandle(m_hActiveWeapon());
}

bool CBasePlayer::SetupBones(matrix3x4_t* Matrix, int MaxBones, int Mask, float CurTime) {
	void* pClientRenderable = (void*)(this + 0x4);
	typedef bool(__thiscall* fnSetupBones)(void*, matrix3x4_t*, int, int, float);
	return CallVFunction<fnSetupBones>(pClientRenderable, 13)(pClientRenderable, Matrix, MaxBones, Mask, CurTime);
}

void CBasePlayer::SetupBones_AttachmentHelper() {
	static auto _SetupBones_AttachmentHelper = reinterpret_cast<void(__thiscall*)(void*, void*)>(Utils::PatternScan("client.dll", "55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4"));

	_SetupBones_AttachmentHelper(this, GetStudioHdr());
}

void CBasePlayer::CopyBones(matrix3x4_t* boneMatrix) {
	memcpy(boneMatrix, GetCachedBoneData().Base(), sizeof(matrix3x4_t) * GetCachedBoneData().Count());
}

void CBasePlayer::ClampBonesInBBox(matrix3x4_t* bone_matrix, int bone_mask) {
	static auto clampbones = reinterpret_cast<void(__thiscall*)(CBasePlayer*, matrix3x4_t*, int)>(Utils::PatternScan("client.dll", "56 57 8B F9 89 7C 24 38 83 BF ? ? ? ? ? 75 16 8B 47 04 8D 4F 04 8B 40 20 FF D0", -0x9));

	clampbones(this, bone_matrix, bone_mask);
}

std::array<float, 24>& CBasePlayer::m_flPoseParameter() {
	static int _m_flPoseParameter = NetVars::GetNetVar("DT_BaseAnimating", "m_flPoseParameter");
	return *(std::array<float, 24>*)(this + _m_flPoseParameter);
}

bool CBasePlayer::IsAlive() {
	if (!this || !PlayerResource)
		return false;

	if (this == Cheat.LocalPlayer && (m_iHealth() <= 0 || m_lifeState() != LIFE_ALIVE))
		return false;

	return PlayerResource->m_bAlive()[EntIndex()];

	//if (m_iTeamNum() != 2 && m_iTeamNum() != 3)
	//	return false;

	//if (m_lifeState() != LIFE_ALIVE)
	//	return false;

	//return true;
}

float CBasePlayer::GetMaxDesyncDelta() {
	if (!this)
		return 0.0f;

	auto animstate = GetAnimstate();

	if (!animstate)
		return 0.0f;

	auto speedfactor = max(min(animstate->flWalkSpeedNormalized, 1.0f), 0.0f);
	auto avg_speedfactor = (animstate->flWalkToRunTransition * -0.3f - 0.2f) * speedfactor + 1.0f;

	auto duck_amount = animstate->flDuckAmount;

	if (duck_amount)
	{
		auto max_velocity = max(min(animstate->flCrouchSpeedNormalized, 1.0f), 0.0f);
		auto duck_speed = duck_amount * max_velocity;

		avg_speedfactor += duck_speed * (0.5f - avg_speedfactor);
	}

	return -animstate->flAimYawMax * avg_speedfactor;
}

float& CBasePlayer::m_flLastBoneSetupTime()
{
	static auto invalidate_bone_cache = (uintptr_t)Utils::PatternScan("client.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81");
	static auto last_bone_setup_time = *(uintptr_t*)(invalidate_bone_cache + 0x11);

	return *(float*)((uintptr_t)this + last_bone_setup_time);
}

void CBasePlayer::InvalidateBoneCache() {
	m_flLastBoneSetupTime() = -FLT_MAX;
	m_iMostRecentModelBoneCounter() = UINT_MAX;
}

void CBasePlayer::ForceBoneCache() {
	static int* g_iModelBoneCounter = *reinterpret_cast<int**>(Utils::PatternScan("client.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81", 0xA));

	*g_iModelBoneCounter = m_iMostRecentModelBoneCounter();
}

CStudioHdr* CBasePlayer::GetStudioHdr() {
	static auto studio_hdr = *(uintptr_t*)Utils::PatternScan("client.dll", "8B B7 ?? ?? ?? ?? 89 74 24 20", 0x2);
	return *(CStudioHdr**)((uintptr_t)this + studio_hdr + 0x4);
}

C_CommandContext* CBasePlayer::GetCommandContext() {
	static const uintptr_t _m_CommandContext = *(uintptr_t*)Utils::PatternScan("client.dll", "80 BE ? ? ? ? ? 0F 84 ? ? ? ? C6 86 ? ? ? ? ? F6 86", 0x2);

	return (C_CommandContext*)(this + _m_CommandContext);
}

std::string CBasePlayer::GetName() {
	player_info_t pinfo;
	EngineClient->GetPlayerInfo(EntIndex(), &pinfo);
	return std::string(pinfo.szName);
}

Vector CBasePlayer::GetShootPosition() {
	if (this != Cheat.LocalPlayer)
		return m_vecOrigin() + Vector(0, 0, m_vecMaxs().z - 8.f);

	Vector shoot_position = GetEyePosition();
	ModifyEyePosition(shoot_position);
	return shoot_position;
}

inline float SimpleSpline(float value)
{
	float valueSquared = value * value;

	// Nice little ease-in, ease-out spline-like curve
	return (3 * valueSquared - 2 * valueSquared * value);
}

inline float SimpleSplineRemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = std::clamp(cVal, 0.0f, 1.0f);
	return C + (D - C) * SimpleSpline(cVal);
}

void CBasePlayer::ModifyEyePosition(Vector& eye_position) {
	CCSGOPlayerAnimationState* animstate = GetAnimstate();

	if (!(animstate->bLanding || animstate->flDuckAmount != 0.f))
		return;

	auto head_position = GetHitboxCenter(HITBOX_HEAD, AnimationSystem->GetPredictionMatrix());
	head_position.z += 1.7f;

	if (head_position.z >= eye_position.z)
		return;

	float flLerp = SimpleSplineRemapValClamped(abs(eye_position.z - head_position.z), 4.0f, 10.f, 0.0f, 1.0f);

	eye_position.z = Math::Lerp(eye_position.z, head_position.z, flLerp);
}

void CBasePlayer::SetAbsVelocity(const Vector& vecAbsVelocity) {
	if (vecAbsVelocity == m_vecAbsVelocity())
		return;

	InvalidatePhysicsRecursive(VELOCITY_CHANGED);
	m_iEFlags() &= ~EFL_DIRTY_ABSVELOCITY;

	m_vecAbsVelocity() = vecAbsVelocity;
}

float CBasePlayer::GetMaxSpeed() {
	CBaseCombatWeapon* weapon = GetActiveWeapon();

	if (weapon)
	{
		CCSWeaponData* weaponData = weapon->GetWeaponInfo();

		if (weaponData)
			return m_bIsScoped() ? weaponData->flMaxSpeedAlt : weaponData->flMaxSpeed;
	}

	return 260.0f;
}

void CBasePlayer::DrawServerHitboxes(float duration, bool monoColor) {
	// mov     eax, [edi]
	// mov     ecx, edi
	// push    1
	// call    dword ptr[eax + 0DCh]
	// movss   xmm1, [esp + 0ECh + var_C8]
	// mov     ecx, eax
	// call    DrawServerHitboxes
	static void* drawServerHitboxes = Utils::PatternScan("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");

	void* serverEntity = UTIL_GetServerPlayer(EntIndex());

	if (!serverEntity)
		return;

	__asm {
		mov		ecx, serverEntity
		push	monoColor
		movss	xmm1, duration
		call	drawServerHitboxes
	}
}

bool CBasePlayer::IsHitboxArmored(int hitbox) {
	if (m_ArmorValue() <= 0)
		return false;

	switch (hitbox) {
	case HITBOX_HEAD:
		return m_bHasHelmet();
	case HITBOX_NECK:
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	case HITBOX_LOWER_CHEST:
	case HITBOX_STOMACH:
	case HITBOX_PELVIS:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_LEFT_HAND:
	case HITBOX_RIGHT_HAND:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return true;
	}

	return false;
}