#include "GrenadePrediction.h"
#include "../../SDK/Interfaces.h"
#include "../../SDK/Globals.h"
#include "../../Utils/Utils.h"

#include <algorithm>
#include <string>

Vector RayCircleIntersection(Vector ray, Vector center, float r) {
	// (x - center.x) ** 2 + (y - center.y) ** 2 = r ** 2
	// ray.y * x = ray.x * y 
	// if std::abs(ray.x) > std::abs(ray.y): y = (ray.y / ray.x) * x else x = (ray.x / ray.y) * y

	if (std::abs(ray.x) > std::abs(ray.y)) {
		float k = ray.y / ray.x;

		// (x - center.x) ** 2 + ((ray.y / ray.x) * x - center.y) ** 2 = r ** 2
		// x ** 2 - 2 * x * center.x + center.x ** 2 + (ray.y / ray.x * x) ** 2 - 2 * (ray.y / ray.x) * x * center.y + center.y * center.y - r ** 2 = 0
		float a = 1 + k * k;
		float b = -2 * center.x - 2 * k * center.y;
		float c = center.Length2DSqr() - r * r;

		float d = b * b - 4 * a * c;

		if (d < 0) { // no intersections, find nearest
			Vector nearest_on_ray = ray * center.Dot(ray);
			Vector diff = (nearest_on_ray - center).Normalized();

			return center + diff * r;
		}
		else if (d < 0.001f) {
			float x = -b / (2 * a);
			float y = k * x;
			return Vector(x, y);
		}

		float d_sqrt = Math::Q_sqrt(d);

		float x = (-b + d_sqrt) / (2 * a);
		float y = k * x;

		Vector dir1(x, y);

		x = (-b - d_sqrt) / (2 * a);
		y = k * x;

		Vector dir2(x, y);

		if (ray.Dot(dir1) > ray.Dot(dir2))
			return dir1;

		return dir2;
	}
	else {
		float k = ray.x / ray.y;

		// (k * y - center.x) ** 2 + (y - center.y) ** 2 = r ** 2
		// (y * k) ** 2 - 2 * k * y * center.x + center.x ** 2 + y ** 2 - 2 * y * center.y + center.y ** 2 - r ** 2 = 0
		float a = 1 + k * k;
		float b = -2 * center.y - 2 * k * center.x;
		float c = center.Length2DSqr() - r * r;

		float d = b * b - 4 * a * c;

		if (d < 0) { // no intersections, find nearest
			Vector nearest_on_ray = ray * center.Dot(ray);
			Vector diff = (nearest_on_ray - center).Normalized();

			return center + diff * r;
		}
		else if (d < 0.001f) {
			float y = -b / (2 * a);
			float x = k * y;
			return Vector(x, y);
		}

		float d_sqrt = Math::Q_sqrt(d);

		float y = (-b + d_sqrt) / (2 * a);
		float x = k * y;

		Vector dir1(x, y);

		y = (-b - d_sqrt) / (2 * a);
		x = k * y;

		Vector dir2(x, y);

		if (ray.Dot(dir1) > ray.Dot(dir2))
			return dir1;

		return dir2;
	}
}

float CalculateThrowYaw(const Vector& wish_dir, const Vector& vel, float throw_velocity, float throw_strength) {
	Vector dir_normalized = wish_dir;
	dir_normalized.z = 0;
	dir_normalized.Normalize();

	float cos_pitch = dir_normalized.Dot(wish_dir) / wish_dir.Q_Length();

	//Vector dir = (wish_dir - vel * 1.25f) / (std::clamp(throw_velocity * 0.9f, 15.f, 750.f) * (std::clamp(throw_strength, 0.f, 1.f) * 0.7f + 0.3f));
	//return Math::VectorAngles_p(dir).yaw;

	Vector real_dir = RayCircleIntersection(dir_normalized, vel * 1.25f, std::clamp(throw_velocity * 0.9f, 15.f, 750.f) * (std::clamp(throw_strength, 0.f, 1.f) * 0.7f + 0.3f) * cos_pitch) - vel * 1.25f;
	return Math::VectorAngles_p(real_dir).yaw;
}

float CalculateThrowPitch(const Vector& wish_dir, float wish_z_vel, const Vector& vel, float throw_velocity, float throw_strength) {
	float speed = std::clamp(throw_velocity * 0.9f, 15.f, 750.f) * (std::clamp(throw_strength, 0.f, 1.f) * 0.7f + 0.3f);
	
	Vector cur_vel = vel * 1.25f + wish_dir * speed;
	Vector wish_vel = Vector(vel.x, vel.y, wish_z_vel) * 1.25f + wish_dir * speed;

	QAngle ang1 = Math::VectorAngles(cur_vel);
	QAngle ang2 = Math::VectorAngles(wish_vel);

	float ang_diff = ang2.pitch - ang1.pitch;

	return ang_diff * (std::cos(DEG2RAD(ang_diff)) + 1) * 0.5f;
}

void GrenadePrediction::PrecacheParticles() {
	Effects->PrecacheParticleSystem("env_fire_tiny_b");
}

void GrenadePrediction::Start() {
	if (!Cheat.InGame || !Cheat.LocalPlayer || !Cheat.LocalPlayer->IsAlive() || (GameRules() && GameRules()->IsFreezePeriod()) || !config.visuals.other_esp.grenade_trajecotry->get()) {
		runningPrediction = false;
		return;
	}

	CBasePlayer* localPlayer = Cheat.LocalPlayer;
	CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)EntityList->GetClientEntityFromHandle(localPlayer->m_hActiveWeapon());

	if (!weapon) {
		runningPrediction = false;
		return;
	}

	weaponId = weapon->m_iItemDefinitionIndex();
	CCSWeaponData* weaponData = weapon->GetWeaponInfo();

	CBaseGrenade* grenade = reinterpret_cast<CBaseGrenade*>(weapon);

	if (!weaponData) {
		runningPrediction = false;
		return;
	}

	if (weaponData->nWeaponType != WEAPONTYPE_GRENADE) {
		runningPrediction = false;
		return;
	}

	if (grenade->m_flThrowTime() > 0 && grenade->m_flThrowTime() < TICKS_TO_TIME(ctx.corrected_tickbase)) {
		runningPrediction = false;
		return;
	}

	Vector vel = Cheat.LocalPlayer->m_vecVelocity();
	if (config.misc.movement.compensate_throwable->get(0) && !(localPlayer->m_fFlags() & FL_ONGROUND))
		vel = (ctx.local_velocity + ctx.last_local_velocity) * 0.5f;

	if (!runningPrediction)
		lerped_velocity = vel;
	else
		lerped_velocity = lerped_velocity + (vel - lerped_velocity) * std::clamp(GlobalVars->frametime * 64.f, 0.1f, 1.f);

	vel = lerped_velocity;

	QAngle viewAngles;
	EngineClient->GetViewAngles(&viewAngles);
	Vector eyePosition = localPlayer->GetAbsOrigin() + localPlayer->m_vecViewOffset();

	const float flThrowStrength = std::clamp(grenade->m_flThrowStrength(), 0.f, 1.f);

	if (config.misc.movement.compensate_throwable->get(1) && !(Cheat.LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		viewAngles.pitch += CalculateThrowPitch(Math::AngleVectors(viewAngles), config.misc.movement.compensate_throwable->get(2) ? 0.f : std::clamp(vel.z, -120.f, 120.f), vel, weaponData->flThrowVelocity, flThrowStrength);
	}

	if (config.misc.movement.compensate_throwable->get(0) && config.misc.movement.compensate_throwable->get(2)) {
		Vector direction = Math::AngleVectors(viewAngles);
		Vector base_vel = direction * (std::clamp(weaponData->flThrowVelocity * 0.9f, 15.f, 750.f) * (flThrowStrength * 0.7f + 0.3f));
		Vector curent_vel = vel * 1.25f + base_vel;

		if (curent_vel.Dot(direction) > 0.f)
			viewAngles.yaw = CalculateThrowYaw(direction, vel, weaponData->flThrowVelocity, flThrowStrength);
	}


	viewAngles.pitch -= (90.f - fabsf(viewAngles.pitch)) * 10.f / 90.f;

	Vector direction;
	Utils::AngleVectors(viewAngles, direction);

	if (vel.LengthSqr() < 20)
		vel = Vector();

	float throw_offset = TICKS_TO_TIME(ctx.tickbase_shift == 13 ? 2 : 7);
	Vector src = eyePosition + Vector(0.f, 0.f, flThrowStrength * 12.f - 12.f) + vel * throw_offset;

	Vector dest = src;
	dest += direction * 22.f;

	CGameTrace trace;
	TraceHull(src, dest, MASK_SOLID | CONTENTS_CURRENT_90, localPlayer, COLLISION_GROUP_NONE, trace);

	runningPrediction = true;
	owner = Cheat.LocalPlayer;

	Predict(trace.endpos - direction * 6.f, direction * (std::clamp(weaponData->flThrowVelocity * 0.9f, 15.f, 750.f) * (flThrowStrength * 0.7f + 0.3f)) + vel * 1.25f, TICKS_TO_TIME(ctx.corrected_tickbase), 0);
}

void GrenadePrediction::Draw() {
	if (!runningPrediction || pathPoints.size() < 2u) {
		if (pMolotovParticle) {
			pMolotovParticle->Stop();
			pMolotovParticle = nullptr;
		}

		return;
	}

	std::vector<Vector2> scrPoints;
	Vector* renderPath = pathPoints.data();
	int pathSize = pathPoints.size();

	for (int i = 0; i < pathSize; i++) {
		if (i % 2 == 1 && i != pathSize - 1)
			continue;
		Vector2 cpoint = Render->WorldToScreen(renderPath[i]);
		if (!cpoint.Invalid())
			scrPoints.push_back(cpoint);
	}

	bool hit = false;
	std::string additional_info = "";

	if (weaponId == HeGrenade) {
		int maxDamage = 0;
		for (int i = 0; i < ClientState->m_nMaxClients; i++) {
			CBasePlayer* player = (CBasePlayer*)EntityList->GetClientEntity(i);

			if (!player || player->IsTeammate() || !player->IsPlayer() || !player->IsAlive() || player->m_bDormant())
				continue;

			int dmg = CalcDamage(vecDetonate, player);

			if (dmg > maxDamage)
				maxDamage = dmg;
		}

		if (maxDamage > 0) {
			additional_info = std::format("-{}hp", maxDamage);
			hit = true;

			int rel_dmg = config.misc.miscellaneous.automatic_grenade_release->get();

			if (maxDamage > rel_dmg && rel_dmg > 0 && reinterpret_cast<CBaseGrenade*>(ctx.active_weapon)->m_bPinPulled())
				ctx.should_release_grenade = true;
		}
	}
	else if (weaponId == Molotov || weaponId == IncGrenade) {
		float minDistance = 10.f;

		for (int i = 0; i < ClientState->m_nMaxClients; i++) {
			CBasePlayer* player = (CBasePlayer*)EntityList->GetClientEntity(i);

			if (!player || player->IsTeammate() || !player->IsPlayer() || !player->IsAlive() || player->m_bDormant())
				continue;

			CGameTrace tr = EngineTrace->TraceRay(vecDetonate + Vector(0, 0, 16), player->m_vecOrigin() + Vector(0, 0, 32), 0x1, player);

			if (tr.fraction == 1.f) {
				float dist = (player->m_vecOrigin() - vecDetonate).Q_Length2D() / 12.f;

				if (dist < minDistance) {
					additional_info = std::format("{:.2}ft", dist);
					hit = true;
					minDistance = dist;
				}
			}
		}

		if (minDistance < 5.f && config.misc.miscellaneous.automatic_grenade_release->get() > 0 && reinterpret_cast<CBaseGrenade*>(ctx.active_weapon)->m_bPinPulled())
			ctx.should_release_grenade = true;
	}

	Render->PolyLine(scrPoints, hit ? config.visuals.other_esp.grenade_trajectory_hit_color->get() : config.visuals.other_esp.grenade_trajectory_color->get());

	for (auto& point : collisionPoints) {
		Vector2 w2s = Render->WorldToScreen(point);
		Render->CircleFilled(w2s, 3, Color(0, 0, 0, 190));
		Render->CircleFilled(w2s, 2, Color(255, 255, 255, 255));
	}

	if (weaponId == Molotov || weaponId == IncGrenade) {
		Ray_t ray;
		CGameTrace trace;
		CTraceFilter filter;
		ray.Init(vecDetonate + Vector(0, 0, 10), vecDetonate - Vector(0, 0, 128));
		filter.pSkip = Cheat.LocalPlayer;

		EngineTrace->TraceRay(ray, 0x1, &filter, &trace);

		if (trace.fraction < 1.f) {
			Render->CircleFilled(Render->WorldToScreen(vecDetonate), 3, Color(255, 0, 0));

			if (config.visuals.other_esp.particles->get(0)) {
				if (!pMolotovParticle) {
					pMolotovParticle = Effects->DispatchParticleEffect("env_fire_tiny_b", vecDetonate, QAngle());
				}
				else {
					pMolotovParticle->SetOrigin(vecDetonate);
				}
			}
			else if (pMolotovParticle) {
				pMolotovParticle->Stop();
				pMolotovParticle = nullptr;
			}
		}
		else {
			hit = false;
		}
	}
	else {
		if (pMolotovParticle) {
			pMolotovParticle->Stop();
			pMolotovParticle = nullptr;
		}

		Render->CircleFilled(Render->WorldToScreen(vecDetonate), 3, Color(255, 0, 0));
	}


	if (!additional_info.empty() && hit) {
		auto sc_pos = Render->WorldToScreen(vecDetonate);
		if (!sc_pos.Invalid())
			Render->Text(additional_info, sc_pos + Vector2(0, 16), Color(245, 245), Verdana, TEXT_CENTERED);
	}
}

void GrenadePrediction::Predict(const Vector& orign, const Vector& velocity, float throwTime, int offset) {
	vecOrigin = orign;
	vecVelocity = velocity;
	nCollisionGroup = COLLISION_GROUP_PROJECTILE;
	nBouncesCount = 0;
	pLastHitEntity = 0;
	pathPoints.clear();
	collisionPoints.clear();
	detonate = false;

	const auto m_nTick = TIME_TO_TICKS(1.f / 30.f);
	m_nLastUpdateTick = -m_nTick;

	switch (weaponId) {
	case SmokeGrenade:
		nextThinkTick = TIME_TO_TICKS(1.5f);
		break;
	case Decoy:
		nextThinkTick = TIME_TO_TICKS(2.f);
		break;
	case Flashbang:
	case HeGrenade:
		flDetonateTime = 1.5f;
		nextThinkTick = TIME_TO_TICKS(0.02f);
		break;
	case IncGrenade:
	case Molotov:
		flDetonateTime = cvars.molotov_throw_detonate_time->GetFloat();
		nextThinkTick = TIME_TO_TICKS(0.02f);
		break;
	}

	flThrowTime = throwTime;

	for (nTick = 0; nTick < TIME_TO_TICKS(60.f); ++nTick) {
		if (nextThinkTick <= nTick)
			Think();

		if (nTick < offset)
			continue;

		if (PhysicsSimulate())
			break;

		pathPoints.push_back(vecOrigin);
		m_nLastUpdateTick = nTick;
	}

	flExpireTime = flThrowTime + TICKS_TO_TIME(nTick);
	pathPoints.push_back(vecDetonate);
}

void GrenadePrediction::Think() {
	switch (weaponId) {
	case SmokeGrenade:
		if (vecVelocity.Length() <= 0.1f) {
			vecDetonate = vecOrigin;
			detonate = true;
		}
		break;
	case Decoy:
		if (vecVelocity.Length() <= 0.2f) {
			vecDetonate = vecOrigin;
			detonate = true;
		}
		break;
	case Flashbang:
	case HeGrenade:
	case IncGrenade:
	case Molotov:
		if (TICKS_TO_TIME(nTick) > flDetonateTime) {
			vecDetonate = vecOrigin;
			detonate = true;
		}
		break;
	}

	nextThinkTick = nTick + TIME_TO_TICKS(0.2f);
}

bool GrenadePrediction::PhysicsSimulate() {
	const float flNewVelocityZ = vecVelocity.z - (cvars.sv_gravity->GetFloat() * 0.4f) * GlobalVars->interval_per_tick;
	Vector vecMove = Vector(
		vecVelocity.x * GlobalVars->interval_per_tick,
		vecVelocity.y * GlobalVars->interval_per_tick,
		(vecVelocity.z + flNewVelocityZ) / 2.f * GlobalVars->interval_per_tick
	);

	vecVelocity.z = flNewVelocityZ;

	CGameTrace trace;

	PhysicsPushEntity(vecMove, trace);

	if (detonate) {
		return true;
	}

	if (trace.fraction != 1.f) {
		pathPoints.push_back(vecOrigin);
		m_nLastUpdateTick = nTick;
		collisionPoints.push_back(vecOrigin);
		PerformFlyCollisionResolution(trace);
	}

	return false;
}

void GrenadePrediction::TraceHull(Vector& src, Vector& end, uint32_t mask, void* ignore, int collisionGroup, CGameTrace& trace) {
	static auto pTraceFilterSimple = Utils::PatternScan("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52", 0x3D);

	std::uintptr_t filter[4] = {
			*reinterpret_cast<std::uintptr_t*> (pTraceFilterSimple),
			reinterpret_cast<std::uintptr_t>   (ignore),
			 (uintptr_t)(nCollisionGroup),
			0
	};

	auto ray = Ray_t();

	ray.Init(src, end, Vector(-2, -2, -2), Vector(2, 2, 2));

	return EngineTrace->TraceRay(ray, mask, (CTraceFilter*)(filter), &trace);
}

void GrenadePrediction::TraceLine(Vector& src, Vector& end, uint32_t mask, void* ignore, int collisionGroup, CGameTrace& trace) {
	static auto pTraceFilterSimple = Utils::PatternScan("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52", 0x3D);

	std::uintptr_t filter[4] = {
			*reinterpret_cast<std::uintptr_t*> (pTraceFilterSimple),
			reinterpret_cast<std::uintptr_t>   (ignore),
			(uintptr_t)(nCollisionGroup),
			0
	};

	auto ray = Ray_t();
	ray.Init(src, end);
	return EngineTrace->TraceRay(ray, mask, (CTraceFilter*)(filter), &trace);
}

void GrenadePrediction::PhysicsTraceEntity(Vector& vecSrc, Vector& vecDst, uint32_t nMask, CGameTrace& pTrace) {
	TraceHull(vecSrc, vecDst, nMask, owner, nCollisionGroup, pTrace);

	if (pTrace.startsolid && (pTrace.contents & CONTENTS_CURRENT_90)) {
		ClearTrace(pTrace);

		TraceHull(vecSrc, vecDst, nMask & ~CONTENTS_CURRENT_90, Cheat.LocalPlayer, nCollisionGroup, pTrace);
	}

	if (!pTrace.DidHit() || !pTrace.hit_entity || !reinterpret_cast<CBaseEntity*>(pTrace.hit_entity)->IsPlayer())
		return;

	ClearTrace(pTrace);
	TraceLine(vecSrc, vecDst, nMask, owner, nCollisionGroup, pTrace);
}

void GrenadePrediction::PhysicsPushEntity(Vector& vecPush, CGameTrace& pTrace) {
	Vector temp = vecOrigin + vecPush;
	PhysicsTraceEntity(vecOrigin, temp, nCollisionGroup == COLLISION_GROUP_DEBRIS ? (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER : MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CURRENT_90 | CONTENTS_HITBOX, pTrace);

	if (pTrace.startsolid) {
		nCollisionGroup = COLLISION_GROUP_INTERACTIVE_DEBRIS;
		temp = vecOrigin - vecPush;
		Vector temp2 = vecOrigin + vecPush;
		TraceLine( temp, temp2, (MASK_SOLID | CONTENTS_CURRENT_90) & ~CONTENTS_MONSTER, owner, nCollisionGroup, pTrace);
	}

	if (pTrace.fraction)
		vecOrigin = pTrace.endpos;
	if (!pTrace.hit_entity)
		return;

	if (reinterpret_cast<CBaseEntity*>(pTrace.hit_entity)->IsPlayer() || weaponId != TaGrenade && weaponId != Molotov && weaponId != IncGrenade)
		return;


	if (weaponId != TaGrenade && pTrace.plane.normal.z < 0.866025403f)
		return;

	detonate = true;
	vecDetonate = vecOrigin;
}

void GrenadePrediction::PerformFlyCollisionResolution(CGameTrace& pTrace) {
	auto flSurfaceElasticity = 1.f;

	if (pTrace.hit_entity) {
		CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(pTrace.hit_entity);
		if (pEntity->IsBreakable()) {
			vecVelocity *= 0.4f;
			return;
		}

		const auto bIsPlayer = pEntity->IsPlayer();
		if (bIsPlayer)
			flSurfaceElasticity = 0.3f;

		if (pEntity->EntIndex()) {
			if (bIsPlayer && pLastHitEntity == pEntity) {
				nCollisionGroup = COLLISION_GROUP_DEBRIS;
				return;
			}

			pLastHitEntity = (CBaseEntity*)pTrace.hit_entity;
		}
	}

	auto vecVelocity1 = Vector();
	const auto vecBackOff = vecVelocity.Dot(pTrace.plane.normal) * 2.f;

	for (int i = 0; i < 3; i++) {
		const auto change = pTrace.plane.normal[i] * vecBackOff;

		vecVelocity1[i] = vecVelocity[i] - change;

		if (std::fabs(vecVelocity[i]) >= 1.f)
			continue;

		vecVelocity1[i] = 0.f;
	}

	vecVelocity1 *= std::clamp < float >(flSurfaceElasticity * 0.45f, 0.f, 0.9f);

	if (pTrace.plane.normal.z > 0.7f) {
		const auto speed_sqr = vecVelocity1.LengthSqr();
		if (speed_sqr > 96000.f) {
			const auto l = vecVelocity1.Normalized().Dot(pTrace.plane.normal);
			if (l > 0.5f)
				vecVelocity1 *= 1.f - l + 0.5f;
		}

		if (speed_sqr < 400.f)
			vecVelocity = Vector();
		else {
			vecVelocity = vecVelocity1;
			Vector tempv = vecVelocity1 * ((1.f - pTrace.fraction) * GlobalVars->interval_per_tick);
			PhysicsPushEntity(tempv, pTrace);
		}
	}
	else {
		vecVelocity = vecVelocity1;
		Vector tempv = vecVelocity1 * ((1.f - pTrace.fraction) * GlobalVars->interval_per_tick);
		PhysicsPushEntity(tempv, pTrace);
	}

	if (nBouncesCount > 20) {
		detonate = true;
		vecDetonate = vecOrigin;
	}

	++nBouncesCount;
}

void GrenadePrediction::ClearTrace(CGameTrace& trace) {
	memset(&trace, 0, sizeof(trace));
	trace.fraction = 1.f;
	trace.fractionleftsolid = 0;
	trace.surface = { "**empty**", 0 };
}

void GrenadeWarning::Warning(CBaseGrenade* entity, int weapId) {
	if (!Cheat.InGame || !config.visuals.other_esp.grenade_proximity_warning->get())
		return;

	pathPoints.clear();
	collisionPoints.clear();

	owner = (CBasePlayer*)EntityList->GetClientEntityFromHandle(entity->m_hThrower());
	weaponId = weapId;

	flThrowTime = entity->GetCreationTime();
	float simulationTime = entity->m_flSimulationTime();

	Predict(entity->m_vecOrigin(), entity->m_vecVelocity(), flThrowTime, TIME_TO_TICKS(simulationTime - flThrowTime));

	float timeInAir = flExpireTime - flThrowTime;

	if (flExpireTime <= simulationTime)
		return;

	std::vector<Vector2> scrPoints = { Render->WorldToScreen(entity->GetAbsOrigin()) };
	bool shouldDrawCircle = true;

	if (weapId == Molotov) {
		CGameTrace trace;
		CTraceFilter filter;
		Ray_t ray;
		ray.Init(vecDetonate + Vector(0, 0, 10), vecDetonate - Vector(0, 0, 128));
		filter.pSkip = owner;

		EngineTrace->TraceRay(ray, 0x1, &filter, &trace);

		if (trace.fraction == 1.f) {
			shouldDrawCircle = false;
		}
		else {
			pathPoints.push_back(trace.endpos);
			vecDetonate = trace.endpos;
		}
	}

	Vector2 pos = Render->WorldToScreen(vecDetonate);

	for (int i = 1; i < pathPoints.size(); i++) {
		Vector start = pathPoints[i - 1];
		Vector end = pathPoints[i];
		GlowObjectManager->AddGlowBox(end, Math::VectorAngles(start - end), Vector(0, -0.5, -0.5), Vector((start - end).Q_Length(), 0.5, 0.5), config.visuals.other_esp.grenade_predict_color->get(), GlobalVars->frametime * 3.f);
	}

	//for (int i = 0; i < pathPoints.size(); i++) {
	//	Vector2 cpoint = Render->WorldToScreen(pathPoints[i]);
	//	if (!cpoint.Invalid())
	//		scrPoints.push_back(cpoint);
	//}

	//Render->PolyLine(scrPoints, Color(240, 161, 14));

	float distance = (Cheat.LocalPlayer->GetEyePosition() - vecDetonate).Q_Length();

	if (!shouldDrawCircle || distance > 700)
		return;

	float distance_alpha = std::clamp(1.f - (distance - 600.f) / 100.f, 0.f, 1.f);

	Render->CircleFilled(pos, 30, Color(16, 16, 16, 190 * distance_alpha));
	Render->GlowCircle2(pos, 27, Color(40, 40, 40, 255 * distance_alpha), Color(20, 20, 20, 255 * distance_alpha));

	if (weapId == HeGrenade) {
		float damage = CalcDamage(vecDetonate + Vector(0, 0, 0.25f), Cheat.LocalPlayer);
		if (damage > 0)
			Render->GlowCircle(pos, 25, Color(255, 50, 50, min(damage / Cheat.LocalPlayer->m_iHealth() * 2.f, 1) * 255));
		Render->Text(std::to_string(int(damage)).c_str(), pos + Vector2(0, 13), Color(255, 255, 255, 255 * distance_alpha), Verdana, TEXT_CENTERED | TEXT_DROPSHADOW);

		Render->Image(Resources::HeGrenade, pos - Vector2(10, 21), Color(255, 255, 255, 230 * distance_alpha));
	}

	if (weapId == Molotov) {
		float distance = (vecDetonate - Cheat.LocalPlayer->GetHitboxCenter(2)).Length2D();
		Render->GlowCircle(pos, 25, Color(255, 50, 50, std::clamp((430 - distance) / 250.f, 0.f, 1.f) * 255));

		Render->Image(Resources::Molotov, pos - Vector2(10, 18), Color(255, 255, 255, 230 * distance_alpha));
	}
}


int GrenadePrediction::CalcDamage(Vector pos, CBasePlayer* target) {
	Vector vecPelvis = target->GetHitboxCenter(2);
	Vector delta = vecPelvis - pos;

	if (delta.Length() > 350)
		return 0;

	CTraceFilter filter;
	CGameTrace trace;
	Ray_t ray;
	filter.pSkip = Cheat.LocalPlayer;
	ray.Init(pos, vecPelvis);

	EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	if (trace.hit_entity != target && trace.fraction != 1.0f)
		return 0;

	static float a = 105.0f;
	static float b = 25.0f;
	static float c = 140.0f;

	float d = ((delta.Length() - b) / c);
	float flDamage = a * exp(-d * d);

	// do main damage calculation here
	auto dmg = max(static_cast<int>(ceilf(CSGO_Armor(flDamage, target->m_ArmorValue()))), 0);

	// clip max damage.
	dmg = min(dmg, (target->m_ArmorValue() > 0) ? 57 : 98);

	return dmg;
}