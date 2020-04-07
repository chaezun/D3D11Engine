#pragma once

class RayHit final
{
public:
	RayHit(const std::shared_ptr<class Actor>& actor, const Vector3& position, const float& distance, const bool& is_inside)
		: actor(actor)
		, position(position)
		, distance(distance)
		, is_inside(is_inside)
	{

	}

public:
	std::shared_ptr<class Actor> actor;
	Vector3 position = Vector3::Zero;
	float distance = 0.0f;
	bool is_inside = false;
};

class Ray final
{
public:
	Ray() = default;
	Ray(const Vector3& start, const Vector3& end);
	~Ray() = default;

	auto Trace(class Context* context) const -> const std::vector<RayHit>;
	auto HitDistance(const BoundBox& bound_box) const -> const float;

	auto GetStart() const-> const Vector3& { return start; }
	auto GetEnd() const-> const Vector3& { return end; }
	auto GetDirection() const-> const Vector3& { return direction; }
	auto GetLength() const-> const float& { return length; }

private:
	Vector3 start = Vector3::Zero;
	Vector3 end = Vector3::Zero;
	Vector3 direction = Vector3::Zero;
	float length = 0.0f;
};