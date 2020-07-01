#pragma once
#include "IComponent.h"

enum class ProjectionType : uint
{
	Perspective,
	Orthographic,

};

class Camera final : public IComponent
{
public:
	Camera
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Camera() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//=================================================================================================
	// [Property]
	//=================================================================================================

	auto GetViewMatrix() const -> const Matrix& { return view; }
	auto GetProjectionMatrix() const -> const Matrix& { return proj; }

	auto GetProjectionType() const -> const ProjectionType& { return projection_type; }
	void SetProjectionType(const ProjectionType& projection_type) { this->projection_type = projection_type; }

	auto GetFOV() const -> const float& { return fov; }
	void SetFOV(const float& fov) { this->fov = fov; }

	auto GetNearPlane() const -> const float& { return near_plane; }
	void SetNearPlane(const float& near_plane) { this->near_plane = near_plane; }

	auto GetFarPlane() const -> const float& { return far_plane; }
	void SetFarPlane(const float& far_plane) { this->far_plane = far_plane; }

	//=================================================================================================
	// [Raycasting]
	//=================================================================================================
	auto GetPickingRay() const -> const Ray& { return ray; }
	auto GetCameraRay() -> const Ray;

	auto Pick(const Vector2& mouse_position, std::shared_ptr<Actor>& actor) -> const bool;

	auto WorldToScreenPoint(const Vector3& world_position) -> const Vector2;
	auto ScreenToWorldPoint(const Vector2& screen_position) -> const Vector3;


private:
	void UpdateViewMatirx();
	void UpdateProjectionMatrix();

private:
	class Renderer* renderer = nullptr;
	class Input* input = nullptr;
	class Timer* timer = nullptr;

	ProjectionType projection_type = ProjectionType::Perspective;
	float fov = Math::ToRadian(45);//field of view
	float near_plane = 0.3f;
	float far_plane = 1000.0f;

	float accelation = 1.0f;
	float drag = accelation * 0.8f;
	Vector3 movement_speed = Vector3::Zero;

	Matrix view;
	Matrix proj;

	Ray ray;

};
