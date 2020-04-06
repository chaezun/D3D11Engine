#include "Framework.h"
#include "ITexture.h"
#include "Importer/TextureImporter.h"

ITexture::ITexture(Context * context, const ResourceType& resource_type)
	: IResource(context, resource_type)
{
	device = context->GetSubsystem<Graphics>()->GetDevice();
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
}

ITexture::~ITexture()
{
	SAFE_RELEASE(shader_resource_view);
	SAFE_RELEASE(unordered_access_view);
	SAFE_RELEASE(render_target_view);

	for (auto depth_stencil_view : depth_stencil_views)
		SAFE_RELEASE(depth_stencil_view);
}

const bool ITexture::SaveToFile(const std::string & path)
{
	uint byte_count = 0;

	if (FileSystem::IsExistFile(path))
	{
		auto stream = std::make_unique<FileStream>(path, FILE_STREAM_READ);
		if (stream->IsOpen())
			stream->Read(byte_count);
	}

	auto is_append = true;
	auto stream = std::make_unique<FileStream>(path, FILE_STREAM_WRITE | FILE_STREAM_APPEND);
	if (!stream->IsOpen())
		return false;

	if (byte_count != 0 && mip_chain.empty())
		stream->Skip(sizeof(uint) + sizeof(uint) + byte_count);
	else
	{
		byte_count = GetByteCountFromMipChain();

		stream->Write(byte_count);
		stream->Write(static_cast<uint>(mip_chain.size()));

		for (auto& mip : mip_chain)
			stream->Write(mip);

		ClearMipChain();
	}

	stream->Write(bpp);
	stream->Write(bpc);
	stream->Write(width);
	stream->Write(height);
	stream->Write(channels);
	stream->Write(is_gray_scale);
	stream->Write(is_transparent);
	stream->Write(resource_name);
	stream->Write(resource_path);

	return true;
}

const bool ITexture::LoadFromFile(const std::string & path)
{
	auto relative_path = FileSystem::GetRelativeFromPath(path);
	if (!FileSystem::IsExistFile(relative_path))
	{
		LOG_ERROR_F("Path \"%s\" is invalid", relative_path.c_str());
		return false;
	}

	ClearMipChain();

	bool is_loading = false;
	if (FileSystem::GetExtensionFromPath(relative_path) == TEXTURE_BIN_EXTENSION)
		is_loading = Load_Native(relative_path);
	else if (FileSystem::IsSupportedTextureFile(relative_path))
		is_loading = Load_Foreign(relative_path, is_generate_mip_map);

	if (!is_loading)
	{
		LOG_ERROR_F("Failed to load \"%s\"", relative_path.c_str());
		return false;
	}

	if (width == 0 || height == 0 || channels == 0 || mip_chain.empty() || mip_chain.front().empty())
	{
		LOG_ERROR("Invalid parameter");
		return false;
	}

	if (!CreateGpuResource())
	{
		LOG_ERROR_F("Failed to create gpu resource for \"%s\"", resource_path.c_str());
		return false;
	}

	if (FileSystem::GetExtensionFromPath(relative_path) == TEXTURE_BIN_EXTENSION)
		ClearMipChain();

	return true;
}

void ITexture::SetViewport(const float & x, const float & y, const float & width, const float & height)
{
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

auto ITexture::Load_Foreign(const std::string & path, const bool & is_generate_mip_map) -> const bool
{
	auto importer = context->GetSubsystem<ResourceManager>()->GetTextureImporter();

	if (!importer->Load(path, this))
	{
		LOG_ERROR("Failed loading texture");
		return false;
	}

	SetResourcePath(FileSystem::GetPathWithoutExtension(path) + TEXTURE_BIN_EXTENSION);
	SetResourceName(FileSystem::GetIntactFileNameFromPath(GetResourcePath()));

	return true;
}

auto ITexture::Load_Native(const std::string & path) -> const bool
{
	auto stream = std::make_unique<FileStream>(path, FILE_STREAM_READ);
	if (!stream->IsOpen())
		return false;

	ClearMipChain();

	auto byte_count = stream->Read<uint>();
	auto mip_count = stream->Read<uint>();

	mip_chain.reserve(mip_count);
	mip_chain.resize(mip_count);

	for (auto& mip : mip_chain)
		stream->Read(mip);

	stream->Read(bpp);
	stream->Read(bpc);
	stream->Read(width);
	stream->Read(height);
	stream->Read(channels);
	stream->Read(is_gray_scale);
	stream->Read(is_transparent);
	stream->Read(resource_name);
	stream->Read(resource_path);

	return true;
}

void ITexture::GetMipChain(std::vector<mip_level>* mip_chain)
{
	if (!this->mip_chain.empty())
	{
		mip_chain = &this->mip_chain;
		return;
	}

	auto stream = std::make_unique<FileStream>(resource_path, FILE_STREAM_READ);
	if (!stream->IsOpen())
		return;

	stream->Skip(sizeof(uint));

	auto mip_count = stream->Read<uint>();
	mip_chain->reserve(mip_count);
	mip_chain->resize(mip_count);

	for (auto& mip : *mip_chain)
		stream->Read(mip);
}

void ITexture::ClearMipChain()
{
	for (auto& mip : mip_chain)
	{
		mip.clear();
		mip.shrink_to_fit();
	}

	mip_chain.clear();
	mip_chain.shrink_to_fit();
}

auto ITexture::GetMipLevel(const uint & level) -> mip_level *
{
	if (level >= static_cast<uint>(mip_chain.size()))
	{
		LOG_ERROR("Mip level out of range");
		return nullptr;
	}

	return &mip_chain[level];
}

auto ITexture::GetByteCountFromMipChain() -> const uint
{
	uint byte_count = 0;

	for (auto& mip : mip_chain)
		byte_count += static_cast<uint>(mip.size());

	return byte_count;
}

auto ITexture::GetChannelsFromFormat(const DXGI_FORMAT & format) -> const uint
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return 4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:    return 4;
	case DXGI_FORMAT_R32G32B32A32_UINT:     return 4;
	case DXGI_FORMAT_R32G32B32A32_SINT:     return 4;
	case DXGI_FORMAT_R32G32B32_TYPELESS:    return 3;
	case DXGI_FORMAT_R32G32B32_FLOAT:       return 3;
	case DXGI_FORMAT_R32G32B32_UINT:        return 3;
	case DXGI_FORMAT_R32G32B32_SINT:        return 3;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return 4;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:    return 4;
	case DXGI_FORMAT_R16G16B16A16_UNORM:    return 4;
	case DXGI_FORMAT_R16G16B16A16_UINT:     return 4;
	case DXGI_FORMAT_R16G16B16A16_SNORM:    return 4;
	case DXGI_FORMAT_R16G16B16A16_SINT:     return 4;
	case DXGI_FORMAT_R32G32_TYPELESS:       return 2;
	case DXGI_FORMAT_R32G32_FLOAT:          return 2;
	case DXGI_FORMAT_R32G32_UINT:           return 2;
	case DXGI_FORMAT_R32G32_SINT:           return 2;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM:        return 4;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return 4;
	case DXGI_FORMAT_R8G8B8A8_UINT:         return 4;
	case DXGI_FORMAT_R8G8B8A8_SNORM:        return 4;
	case DXGI_FORMAT_R8G8B8A8_SINT:         return 4;
	case DXGI_FORMAT_R16G16_TYPELESS:       return 2;
	case DXGI_FORMAT_R16G16_FLOAT:          return 2;
	case DXGI_FORMAT_R16G16_UNORM:          return 2;
	case DXGI_FORMAT_R16G16_UINT:           return 2;
	case DXGI_FORMAT_R16G16_SNORM:          return 2;
	case DXGI_FORMAT_R16G16_SINT:           return 2;
	case DXGI_FORMAT_R32_TYPELESS:          return 1;
	case DXGI_FORMAT_D32_FLOAT:             return 1;
	case DXGI_FORMAT_R32_FLOAT:             return 1;
	case DXGI_FORMAT_R32_UINT:              return 1;
	case DXGI_FORMAT_R32_SINT:              return 1;
	case DXGI_FORMAT_R8G8_TYPELESS:         return 2;
	case DXGI_FORMAT_R8G8_UNORM:            return 2;
	case DXGI_FORMAT_R8G8_UINT:             return 2;
	case DXGI_FORMAT_R8G8_SNORM:            return 2;
	case DXGI_FORMAT_R8G8_SINT:             return 2;
	case DXGI_FORMAT_R16_TYPELESS:          return 1;
	case DXGI_FORMAT_R16_FLOAT:             return 1;
	case DXGI_FORMAT_D16_UNORM:             return 1;
	case DXGI_FORMAT_R16_UNORM:             return 1;
	case DXGI_FORMAT_R16_UINT:              return 1;
	case DXGI_FORMAT_R16_SNORM:             return 1;
	case DXGI_FORMAT_R16_SINT:              return 1;
	case DXGI_FORMAT_R8_TYPELESS:           return 1;
	case DXGI_FORMAT_R8_UNORM:              return 1;
	case DXGI_FORMAT_R8_UINT:               return 1;
	case DXGI_FORMAT_R8_SNORM:              return 1;
	case DXGI_FORMAT_R8_SINT:               return 1;
	default:                                return 0;
	}
}
