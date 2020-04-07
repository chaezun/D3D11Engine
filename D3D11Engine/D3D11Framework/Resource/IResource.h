#pragma once

#include "Core/D3D11/D3D11_Object.h"

enum class ResourceType :uint
{
	Unknown,
	Texture2D,
	Texture3D,
	TextureCube,
	Model,
	Mesh,
	Material,
};

class IResource : public D3D11_Object
{
public:
    template<typename T>
	static constexpr ResourceType DeduceResourceType();

public:
    //Default 매개변수가 들어갈 경우 오버로딩을 하면 안됨. 이유는 모호성때문
	IResource(class Context* context, const ResourceType& resource_type= ResourceType::Unknown);
	virtual ~IResource()=default;

	virtual const bool SaveToFile(const std::string& path)=0;
	virtual const bool LoadFromFile(const std::string& path)=0;

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

