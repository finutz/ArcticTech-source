#include "IEngineTrace.h"

#include "../../Utils/Utils.h"
#include "../Misc/CBaseEntity.h"
#include "../Misc/CBasePlayer.h"

bool IEngineTrace::ClipRayToHitboxes(const Ray_t& ray, unsigned int fMask, CBaseEntity* pEnt, CGameTrace* pTrace) {
	static auto _ClipRayToHitboxes = reinterpret_cast<bool(__thiscall*)(IEngineTrace*, const Ray_t&, unsigned int, ICollideable*, CGameTrace*)>(Utils::PatternScan("engine.dll", "55 8B EC 83 EC 68 56 57 6A 54 8D 45 98 6A 00 50"));
	return _ClipRayToHitboxes(this, ray, fMask, pEnt->GetCollideable(), pTrace);
}

bool IEngineTrace::ClipRayToPlayer(const Ray_t& ray, unsigned int fMask, CBaseEntity* pEnt, CGameTrace* pTrace) {
	if (!EngineTrace->ClipRayToHitboxes(ray, fMask, pEnt, pTrace))
		return false;

	if (pTrace->DidHit())
		pTrace->hit_entity = pEnt;
	else
		return false;

	return true;
}

bool IEngineTrace::RayIntersectPlayer(const Vector& start, const Vector& end, CBasePlayer* player, matrix3x4_t* bone_matrix, int filter_damagegroup) {
	const auto model = player->GetClientRenderable()->GetModel();

	if (!model)
		return false;

	auto hdr = ModelInfoClient->GetStudioModel(model);

	if (!hdr)
		return false;

	auto set = hdr->GetHitboxSet(player->m_nHitboxSet());

	if (!set)
		return false;

	for (int i = 0; i < set->numhitboxes; ++i) {
		if (filter_damagegroup != -1 && HitboxToDamagegroup(i) != filter_damagegroup)
			continue;

		const auto box = set->GetHitbox(i);

		if (!box)
			continue;

		float radius = box->flCapsuleRadius;
		const auto is_capsule = radius != -1.f;
		Vector min, max;

		if (is_capsule) {
			Math::VectorTransform(box->bbmin, bone_matrix[box->bone], &min);
			Math::VectorTransform(box->bbmax, bone_matrix[box->bone], &max);

			if (SegmentToSegment(start, end, min, max) <= radius)
				return true;
		}
		else {
			Math::VectorTransform(Math::VectorRotate(box->bbmin, box->angOffsetOrientation), bone_matrix[box->bone], &min);
			Math::VectorTransform(Math::VectorRotate(box->bbmax, box->angOffsetOrientation), bone_matrix[box->bone], &max);

			Vector delta;
			Vector start_scaled;

			Math::VectorTransform(start, bone_matrix[box->bone], &start_scaled);
			delta = Math::VectorRotate((end - start).Q_Normalized() * 8192.f, bone_matrix[box->bone]);
			if (IntersectBBHitbox(start_scaled, delta, min, max)) {
				return true;
			}
		}
	}

	return false;
}

Vector IEngineTrace::ClosestPoint(const Vector& start, const Vector& end, const Vector& point) {
	const Vector dir = end - start;
	const Vector diff = point - start;

	const float t = diff.Dot(dir) / dir.LengthSqr();
	const Vector closest_point = start + dir * t;

	return closest_point;
}

float IEngineTrace::DistanceToRay(const Vector& start, const Vector& end, const Vector& point) {
	return (point - ClosestPoint(start, end, point)).Length();
}

float IEngineTrace::SegmentToSegment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2) {
	static auto constexpr epsilon = 0.00000001f;

	auto u = s2 - s1;
	auto v = k2 - k1;
	auto w = s1 - k1;

	auto a = u.Dot(u);
	auto b = u.Dot(v);
	auto c = v.Dot(v);
	auto d = u.Dot(w);
	auto e = v.Dot(w);
	auto D = a * c - b * b;

	auto sn = 0.0f, sd = D;
	auto tn = 0.0f, td = D;

	if (D < epsilon)
	{
		sn = 0.0f;
		sd = 1.0f;
		tn = e;
		td = c;
	}
	else
	{
		sn = b * e - c * d;
		tn = a * e - b * d;

		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if (tn < 0.0f)
	{
		tn = 0.0f;

		if (-d < 0.0f)
			sn = 0.0f;
		else if (-d > a)
			sn = sd;
		else
		{
			sn = -d;
			sd = a;
		}
	}
	else if (tn > td)
	{
		tn = td;

		if (-d + b < 0.0f)
			sn = 0.0f;
		else if (-d + b > a)
			sn = sd;
		else
		{
			sn = -d + b;
			sd = a;
		}
	}

	auto sc = fabs(sn) < epsilon ? 0.0f : sn / sd;
	auto tc = fabs(tn) < epsilon ? 0.0f : tn / td;

	auto dp = w + u * sc - v * tc;
	return dp.Length();
}

bool IEngineTrace::IntersectBBHitbox(const Vector& start, const Vector& delta, const Vector& min, const Vector& max) {
	float d1, d2, f;
	auto start_solid = true;
	auto t1 = -1.0, t2 = 1.0;

	const float _start[3] = { start.x, start.y, start.z };
	const float _delta[3] = { delta.x, delta.y, delta.z };
	const float mins[3] = { min.x, min.y, min.z };
	const float maxs[3] = { max.x, max.y, max.z };

	for (auto i = 0; i < 6; ++i) {
		if (i >= 3) {
			const auto j = (i - 3);

			d1 = _start[j] - maxs[j];
			d2 = d1 + _delta[j];
		}
		else {
			d1 = -_start[i] + mins[i];
			d2 = d1 - _delta[i];
		}

		if (d1 > 0 && d2 > 0) {
			start_solid = false;
			return false;
		}

		if (d1 <= 0 && d2 <= 0)
			continue;

		if (d1 > 0)
			start_solid = false;

		if (d1 > d2) {
			f = d1;
			if (f < 0)
				f = 0;

			f /= d1 - d2;
			if (f > t1)
				t1 = f;
		}
		else {
			f = d1 / (d1 - d2);
			if (f < t2)
				t2 = f;
		}
	}

	return start_solid || (t1 < t2 && t1 >= 0.0f);
}