#include "D3D11Framework.h"
#include "Ray.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Renderable.h"

Ray::Ray(const Vector3 & start, const Vector3 & end)
	: start(start)
	, end(end)
{
	auto start_to_end = end - start;

	length = start_to_end.Length();
	direction = Vector3::Normalize(start_to_end);
}

auto Ray::Trace(Context * context) const -> const std::vector<RayHit>
{
	//광선이 닿는 모든 actor 찾기
	std::vector<RayHit> hits;
	const auto& actors = context->GetSubsystem<SceneManager>()->GetCurrentScene()->GetAllActors();
	for (const auto& actor : actors)
	{
		if (!actor->HasComponent<Renderable>())
			continue;

		const auto& aabb = actor->GetComponent<Renderable>()->GetBoundBox();

		auto hit_distance = HitDistance(aabb);

		if (hit_distance == std::numeric_limits<float>::infinity())
			continue;

		auto hit_position = start + hit_distance * direction;
		hits.emplace_back(actor, hit_position, hit_distance, hit_distance == 0.0f);
	}

	std::sort(hits.begin(), hits.end(), [](const RayHit& lhs, const RayHit& rhs)
	{
		return lhs.distance < rhs.distance;
	});

	return hits;
}

auto Ray::HitDistance(const BoundBox & bound_box) const -> const float
{
	if (!bound_box.IsDefined())
		return std::numeric_limits<float>::infinity();

	if (bound_box.IsInside(start) != Intersection::Outside)
		return 0.0f;

	auto distance = std::numeric_limits<float>::infinity();

	//X 방향으로 교차 검사
	if (start.x < bound_box.GetMin().x && direction.x > 0.0f)
	{
		auto x = (bound_box.GetMin().x - start.x) / direction.x;
		if (x < distance)
		{
			const auto point = start + x * direction;
			if (
				point.y >= bound_box.GetMin().y &&
				point.y <= bound_box.GetMax().y &&
				point.z >= bound_box.GetMin().z &&
				point.z <= bound_box.GetMax().z)
				distance = x;
		}
	}

	if (start.x > bound_box.GetMax().x && direction.x < 0.0f)
	{
		auto x = (bound_box.GetMax().x - start.x) / direction.x;
		if (x < distance)
		{
			const auto point = start + x * direction;
			if (
				point.y >= bound_box.GetMin().y &&
				point.y <= bound_box.GetMax().y &&
				point.z >= bound_box.GetMin().z &&
				point.z <= bound_box.GetMax().z)
				distance = x;
		}
	}

	//Y 방향으로 교차 검사
	if (start.y < bound_box.GetMin().y && direction.y > 0.0f)
	{
		auto y = (bound_box.GetMin().y - start.y) / direction.y;
		if (y < distance)
		{
			const auto point = start + y * direction;
			if (
				point.x >= bound_box.GetMin().x &&
				point.x <= bound_box.GetMax().x &&
				point.z >= bound_box.GetMin().z &&
				point.z <= bound_box.GetMax().z)
				distance = y;
		}
	}

	if (start.y > bound_box.GetMax().y && direction.y < 0.0f)
	{
		auto y = (bound_box.GetMax().y - start.y) / direction.y;
		if (y < distance)
		{
			const auto point = start + y * direction;
			if (
				point.x >= bound_box.GetMin().x &&
				point.x <= bound_box.GetMax().x &&
				point.z >= bound_box.GetMin().z &&
				point.z <= bound_box.GetMax().z)
				distance = y;
		}
	}

	//Z 방향으로 교차 검사
	if (start.z < bound_box.GetMin().z && direction.z > 0.0f)
	{
		auto z = (bound_box.GetMin().z - start.z) / direction.z;
		if (z < distance)
		{
			const auto point = start + z * direction;
			if (
				point.x >= bound_box.GetMin().x &&
				point.x <= bound_box.GetMax().x &&
				point.y >= bound_box.GetMin().y &&
				point.y <= bound_box.GetMax().y)
				distance = z;
		}
	}

	if (start.z > bound_box.GetMax().z && direction.z < 0.0f)
	{
		auto z = (bound_box.GetMax().z - start.z) / direction.z;
		if (z < distance)
		{
			const auto point = start + z * direction;
			if (
				point.x >= bound_box.GetMin().x &&
				point.x <= bound_box.GetMax().x &&
				point.y >= bound_box.GetMin().y &&
				point.y <= bound_box.GetMax().y)
				distance = z;
		}
	}

	return distance;
}
