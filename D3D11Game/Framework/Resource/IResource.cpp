#include "Framework.h"
#include "IResource.h"


IResource::IResource(Context * context, const ResourceType & resource_type)
:context(context), resource_type(resource_type),resource_name(NOT_ASSIGNED_STR), resource_path(NOT_ASSIGNED_STR)
{

}


template<typename T>
constexpr ResourceType IResource::DeduceResourceType()
{
	return ResourceType::Unknown;
}


#define REGISTER_RESOURCE_TYPE(T, enum_type) template<> ResourceType IResource::DeduceResourceType<T>() { return enum_type;}

REGISTER_RESOURCE_TYPE(Texture2D,ResourceType::Texture2D)
REGISTER_RESOURCE_TYPE(Material, ResourceType::Material)
REGISTER_RESOURCE_TYPE(Mesh,     ResourceType::Mesh)
REGISTER_RESOURCE_TYPE(Model, ResourceType::Model)
