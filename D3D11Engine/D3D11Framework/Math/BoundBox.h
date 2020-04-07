#pragma once

class BoundBox final
{
public:
	BoundBox() = default;
	BoundBox(const Vector3& min, const Vector3& max);
	BoundBox(const std::vector<struct VertexTextureNormalTangent>& vertices);
	~BoundBox() = default;

	auto GetMin() const -> const Vector3& { return min; }
	auto GetMax() const -> const Vector3& { return max; }

	auto GetCenter() const -> const Vector3 { return (max + min) * 0.5f; }
	auto GetSize() const -> const Vector3 { return max - min; }
	auto GetExtents() const -> const Vector3 { return (max - min) * 0.5f; }

	auto IsInside(const Vector3& point) const -> const Intersection;
	auto IsInside(const BoundBox& box) const -> const Intersection;

	auto Transformed(const Matrix& transform) -> const BoundBox;
	void Merge(const BoundBox& box);

	void Undefined();
	auto IsDefined() const -> const bool { return min.x != std::numeric_limits<float>::max(); };

private:
	Vector3 min = Vector3::Infinity;
	Vector3 max = Vector3::NegInfinity;
};

//   |-------------------------------|
//   2                               8