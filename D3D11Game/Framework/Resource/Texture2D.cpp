#include "Framework.h"
#include "Texture2D.h"
#include "Core/D3D11/D3D11_Helper.h"

auto Texture2D::CreateTexture(ID3D11Device * device, ID3D11Texture2D ** resource, const uint & width, const uint & height, const uint & channels, const uint & bpc, const uint & array_size, const DXGI_FORMAT & format, const uint & flags, std::vector<std::vector<std::byte>>& data) -> const bool
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = data.empty() ? 1 : static_cast<uint>(data.size());
	desc.ArraySize = array_size;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = (flags & D3D11_BIND_RENDER_TARGET) || (flags & D3D11_BIND_DEPTH_STENCIL) || (flags & D3D11_BIND_UNORDERED_ACCESS) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = flags;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	std::vector<D3D11_SUBRESOURCE_DATA> subresource_datas;
	auto mip_width = width;
	auto mip_height = height;

	for (uint i = 0; i < static_cast<uint>(data.size()); i++)
	{
		if (data[i].empty())
		{
			LOG_ERROR_F("Mipmap %d has invalid data", i);
			return false;
		}

		auto& subresource_data = subresource_datas.emplace_back(D3D11_SUBRESOURCE_DATA{}); //Uniform Initialization
		subresource_data.pSysMem = data[i].data();
		subresource_data.SysMemPitch = mip_width * channels * (bpc / 8);
		subresource_data.SysMemSlicePitch = 0;

		mip_width = Math::Max(mip_width / 2, 1U);
		mip_height = Math::Max(mip_height / 2, 1U);
	}

	const auto result = device->CreateTexture2D(&desc, subresource_datas.data(), resource);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11Texture2D, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::CreateRenderTargetView(ID3D11Device * device, ID3D11Resource * resource, ID3D11RenderTargetView ** render_target_view, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = (array_size == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = array_size;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipSlice = 0;

	const auto result = device->CreateRenderTargetView(resource, &desc, render_target_view);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11RenderTargetView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::CreateDepthStencilView(ID3D11Device * device, ID3D11Resource * resource, std::vector<ID3D11DepthStencilView*>& depth_stencil_views, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = (array_size == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipSlice = 0;

	for (uint i = 0; i < array_size; i++)
	{
		desc.Texture2DArray.FirstArraySlice = i;
		const auto depth_stencil_view = &depth_stencil_views.emplace_back(nullptr);
		const auto result = device->CreateDepthStencilView(resource, &desc, depth_stencil_view);
		if (FAILED(result))
		{
			LOG_ERROR_F("Invalid parameter, failed to create ID3D11DepthStencilView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
			return false;
		}
	}

	return true;
}

auto Texture2D::CreateShaderResourceView(ID3D11Device * device, ID3D11Resource * resource, ID3D11ShaderResourceView ** shader_resource_view, const DXGI_FORMAT & format, const uint & array_size, std::vector<std::vector<std::byte>>& data) -> const bool
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = (array_size == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = array_size;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipLevels = data.empty() ? 1 : static_cast<uint>(data.size());
	desc.Texture2DArray.MostDetailedMip = 0;

	const auto result = device->CreateShaderResourceView(resource, &desc, shader_resource_view);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11ShaderResourceView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::CreateUnorderedAccessView(ID3D11Device * device, ID3D11Resource * resource, ID3D11UnorderedAccessView ** unordered_access_view, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = (array_size == 1) ? D3D11_UAV_DIMENSION_TEXTURE2D : D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = array_size;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipSlice = 0;

	const auto result = device->CreateUnorderedAccessView(resource, &desc, unordered_access_view);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11UnorderedAccessView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::SaveEmbeddedTextureToFile(const std::string & path, const std::string & format, const uint & width, const uint & height, void * data) -> const std::string
{
	const auto texture_path = FileSystem::GetPathWithoutExtension(path) + "." + format;

	auto stream = std::make_unique<FileStream>(texture_path, FILE_STREAM_WRITE);
	if (!stream->IsOpen())
		return NOT_ASSIGNED_STR;

	if (height == 0)
		stream->Write(data, width);
	else
		stream->Write(data, width * height);

	return texture_path;
}

Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const std::vector<std::vector<std::byte>>& data, const uint & flags)
	: ITexture(context, ResourceType::Texture2D)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->channels = GetChannelsFromFormat(format);
	this->mip_chain = data;
	this->bind_flags = static_cast<TEXTURE_BIND_FLAGS>(flags);

	SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

	CreateGpuResource();
}

Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const std::vector<std::byte>& data, const uint & flags)
	: ITexture(context, ResourceType::Texture2D)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->channels = GetChannelsFromFormat(format);
	this->bind_flags = static_cast<TEXTURE_BIND_FLAGS>(flags);
	this->mip_chain.emplace_back(data);

	SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

	CreateGpuResource();
}


Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const uint & array_size, const uint & flags)
	: ITexture(context, ResourceType::Texture2D)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->channels = GetChannelsFromFormat(format);
	this->array_size = array_size;
	this->bind_flags = static_cast<TEXTURE_BIND_FLAGS>(flags);

	SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

	CreateGpuResource();
}

Texture2D::Texture2D(Context * context, const bool & is_generate_mip_map, const uint & flags)
	: ITexture(context, ResourceType::Texture2D)
{
	this->is_generate_mip_map = is_generate_mip_map;
	this->bind_flags = static_cast<TEXTURE_BIND_FLAGS>(flags);
}

const bool Texture2D::CreateGpuResource()
{
	uint final_flags = 0U;

	final_flags |= (bind_flags & TEXTURE_BIND_RTV) ? D3D11_BIND_RENDER_TARGET : 0U;
	final_flags |= (bind_flags & TEXTURE_BIND_DSV) ? D3D11_BIND_DEPTH_STENCIL : 0U;
	final_flags |= (bind_flags & TEXTURE_BIND_SRV) ? D3D11_BIND_SHADER_RESOURCE : 0U;
	final_flags |= (bind_flags & TEXTURE_BIND_UAV) ? D3D11_BIND_UNORDERED_ACCESS : 0U;

	auto buffer_format = format;
	auto dsv_format = format;
	auto srv_format = format;

	if (format == DXGI_FORMAT_D32_FLOAT)
	{
		buffer_format = DXGI_FORMAT_R32_TYPELESS;
		dsv_format = DXGI_FORMAT_D32_FLOAT;
		srv_format = DXGI_FORMAT_R32_FLOAT;
	}

	ID3D11Texture2D* resource = nullptr;
	auto result = CreateTexture
	(
		device,
		&resource,
		width,
		height,
		channels,
		bpc,
		array_size,
		buffer_format,
		final_flags,
		mip_chain
	);

	if (!result)
		return false;

	if (bind_flags & TEXTURE_BIND_RTV)
	{
		result = CreateRenderTargetView
		(
			device,
			resource,
			&render_target_view,
			format,
			array_size
		);

		if (!result)
		{
			SAFE_RELEASE(resource);
			return false;
		}
	}

	if (bind_flags & TEXTURE_BIND_DSV)
	{
		result = CreateDepthStencilView
		(
			device,
			resource,
			depth_stencil_views,
			dsv_format,
			array_size
		);

		if (!result)
		{
			SAFE_RELEASE(resource);
			return false;
		}
	}

	if (bind_flags & TEXTURE_BIND_SRV)
	{
		result = CreateShaderResourceView
		(
			device,
			resource,
			&shader_resource_view,
			srv_format,
			array_size,
			mip_chain
		);

		if (!result)
		{
			SAFE_RELEASE(resource);
			return false;
		}
	}

	if (bind_flags & TEXTURE_BIND_UAV)
	{
		result = CreateUnorderedAccessView
		(
			device,
			resource,
			&unordered_access_view,
			format,
			array_size
		);

		if (!result)
		{
			SAFE_RELEASE(resource);
			return false;
		}
	}

	SAFE_RELEASE(resource);
	return true;
}