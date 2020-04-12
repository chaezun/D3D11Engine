#pragma once

#include "Core/D3D11Graphics/D3D11_Object.h"

enum class ResourceType :uint
{
	Unknown,
	AudioClip,
	Texture2D,
	Texture3D,
	TextureCube,
	Model,
	Mesh,
	Material,
	Skeleton,
	Animation,
};

class IResource : public D3D11_Object
{
public:
	//Resource의 유형을 추론하는 함수. 
	//어떤 유형이 들어올지 확신할 수 없기 때문에 Template로 정의
    template<typename T>
	static constexpr ResourceType DeduceResourceType();

public:
    //Default 매개변수가 들어갈 경우 오버로딩을 하면 안됨. 이유는 모호성때문
	IResource(class Context* context, const ResourceType& resource_type= ResourceType::Unknown);
	virtual ~IResource()=default;

	//특정 경로에 위차한 파일을 저장(오버라이딩)
	virtual const bool SaveToFile(const std::string& path) = 0;
	//특정 경로에 위차한 파일을 실행(오버라이딩)
	virtual const bool LoadFromFile(const std::string& path) = 0;

	auto GetResourceType() const -> const ResourceType& {return resource_type;}
	void SetResourceType(const ResourceType& type) {this->resource_type=type;}

	auto GetResourceName() const ->const std::string& { return resource_name; }
	void SetResourceName(const std::string& name) { this->resource_name = name; }

	auto GetResourcePath() const ->const std::string& { return resource_path; }
	void SetResourcePath(const std::string& path) { this->resource_path = path; }

protected:
    class Context* context;
	ResourceType resource_type;
	std::string resource_name;
	std::string resource_path;
};

