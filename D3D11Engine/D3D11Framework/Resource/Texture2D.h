#pragma once
#include "ITexture.h"

class Texture2D final :public ITexture
{
public:
   static auto CreateTexture
   (
      ID3D11Device* device,
	  ID3D11Texture2D** resource,
	  const uint& width,
	  const uint& height,
	  const uint& channels,
	  const uint& bpc,
	  const uint& array_size,
	  const DXGI_FORMAT& format,
	  const uint& flags,
	  std::vector<std::vector<std::byte>>& data
   ) -> const bool;

   static auto CreateRenderTargetView
   (
	   ID3D11Device* device,
	   ID3D11Resource* resource //ID3D11Texture2D와 같은 의미
	   ,
	   ID3D11RenderTargetView** render_target_view,
	   const DXGI_FORMAT& format,
	   const uint& array_size
   ) -> const bool;

   static auto CreateDepthStencilView
   (
	   ID3D11Device* device,
	   ID3D11Resource* resource //ID3D11Texture2D와 같은 의미
	   ,
	   std::vector<ID3D11DepthStencilView*>& depth_stencil_views,
	   const DXGI_FORMAT& format,
	   const uint& array_size
   ) -> const bool;

   static auto CreateShaderResourceView
   (
	   ID3D11Device* device,
	   ID3D11Resource* resource //ID3D11Texture2D와 같은 의미
	   ,
	   ID3D11ShaderResourceView** shader_resource_view,
	   const DXGI_FORMAT& format,
	   const uint& array_size,
	   std::vector<std::vector<std::byte>>& data
   ) -> const bool;

   static auto CreateUnorderedAccessView
   (
	   ID3D11Device* device,
	   ID3D11Resource* resource //ID3D11Texture2D와 같은 의미
	   ,
	   ID3D11UnorderedAccessView** unordered_access_view,
	   const DXGI_FORMAT& format,
	   const uint& array_size
   ) -> const bool;

   static auto SaveEmbeddedTextureToFile
   (
      const std::string& path,
	  const std::string& format,
	  const uint& width,
	  const uint& height,
	  void* data
   ) -> const std::string;

public:
	Texture2D
	(
	  class Context* context,
	  const uint& width,
	  const uint& height,
	  const DXGI_FORMAT& format,
	  const std::vector<std::vector<std::byte>>& data,
	  const uint& flags= TEXTURE_BIND_SRV
	);

	Texture2D
	(
		class Context* context,
		const uint& width,
		const uint& height,
		const DXGI_FORMAT& format,
		const std::vector<std::byte>& data,
		const uint& flags= TEXTURE_BIND_SRV
	);

	Texture2D
	(
		class Context* context,
		const uint& width,
		const uint& height,
		const DXGI_FORMAT& format,
		const uint& array_size,
		const uint& flags= TEXTURE_BIND_SRV
	);

	Texture2D
	(
		class Context* context,
		const bool& is_generate_mip_map=true,
		const uint& flags=TEXTURE_BIND_SRV
	);

	~Texture2D()=default;

	const bool CreateGpuResource() override;
};