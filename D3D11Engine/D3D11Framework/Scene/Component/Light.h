#pragma once
#include "IComponent.h"

enum class LightType : uint
{
	Directional, //�� ������ �������ִ� ����
	Point, //�� �������� ���� ������ ����
	Spot, //Directional�� ��������� �ϳ��� ���⸸ ������ ���� �ʰ� ���� ������ ������ ����
};

class Light final : public IComponent
{
public:
	Light
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Light() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//=================================================================================================
	// [Property]
	//=================================================================================================
	auto GetDirection() const->Vector3;

	auto GetLightType() const -> const LightType& { return light_type; }
	void SetLightType(const LightType& type);

	auto GetRange() const -> const float& { return range; }
	void SetRange(const float& range);

	auto GetIntensity() const -> const float& { return intensity; }
	void SetIntensity(const float& intensity) { this->intensity = intensity; }

	auto GetAngle() const -> const float& { return angle_radian; }
	void SetAngle(const float& angle);

	auto GetBias() const -> const float& { return bias; }
	void SetBias(const float& bias) { this->bias = bias; }

	auto GetNormalBias() const -> const float& { return normal_bias; }
	void SetNormalBias(const float& normal_bias) { this->normal_bias = normal_bias; }

	auto GetColor() const -> const Color4& { return color; }
	void SetColor(const Color4& color) { this->color = color; }

	auto IsCastShadow() const -> const bool& { return is_cast_shadow; }
	void SetCastShadow(const bool& is_cast_shadow);

	//=================================================================================================
	// [Constant Buffer]
	//=================================================================================================
	auto GetConstantBuffer() const -> const std::shared_ptr<ConstantBuffer>& { return gpu_buffer; }
	void UpdateConstantBuffer();

private:
    LightType light_type=LightType::Point;
	float range=10.0f; //Point, Spot �ʿ�
	float intensity=2.0f; //���� ����
	float angle_radian=0.5f; //Spot �ʿ�
	float bias =0.0f;
	float normal_bias=0.0f;
	bool is_update=false;
	bool is_cast_shadow=false;
	Color4 color=Color4(1.0f, 0.76f, 0.57f, 1.0f);

	//=================================================================================================
	// [Constant Buffer]
	//=================================================================================================
	LIGHT_DATA cpu_buffer;
	std::shared_ptr<ConstantBuffer> gpu_buffer;
};