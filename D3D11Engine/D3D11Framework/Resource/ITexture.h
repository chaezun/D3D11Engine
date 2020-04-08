#pragma once
#include "IResource.h"

//비트단위를 사용하는 이유: 공간의 낭비를 최소화하기 위해 하드웨어의 제어와 상태 확인을 위한
//레지스터에 비트단위로 기능을 할당하기 위함.
enum TEXTURE_BIND_FLAGS : uint //자원뷰는 총 4가지
{
	TEXTURE_BIND_RTV = 1U << 0,//Binary에서 0번째 비트를 1로 설정(..0001), ID3D11RenderTargetView
	TEXTURE_BIND_DSV = 1U << 1,//Binary에서 1번째 비트를 1로 설정(..0010), ID3D11DepthStencilView
	TEXTURE_BIND_SRV = 1U << 2,//Binary에서 2번째 비트를 1로 설정(..0100), ID3D11ShaderResourceView
	TEXTURE_BIND_UAV = 1U << 3,//Binary에서 3번째 비트를 1로 설정(..1000), ID3D11UnorderedAccessView
};

class ITexture : public IResource
{
public: 
   typedef std::vector<std::byte> mip_level;

public: 
   ITexture(class Context* context,const ResourceType& resource_type);
   virtual ~ITexture();

   virtual const bool SaveToFile(const std::string& path) override;
   virtual const bool LoadFromFile(const std::string& path) override;

   //=================================================================================================================
   //[property]
   //=================================================================================================================
   auto GetShaderResourceView() const -> ID3D11ShaderResourceView* { return shader_resource_view; }
   auto GetUnorderedAccessView() const -> ID3D11UnorderedAccessView* { return unordered_access_view; }
   auto GetRenderTargetView() const -> ID3D11RenderTargetView* { return render_target_view; }
   auto GetDepthStencilView(const uint& index=0) const -> ID3D11DepthStencilView* { return index < static_cast<uint>(depth_stencil_views.size()) ?  depth_stencil_views[index]:nullptr;}

   auto GetViewport() const -> const D3D11_VIEWPORT& { return viewport; }
   void SetViewport(const float& x, const float& y, const float& width, const float& height);

   auto GetFormat() const -> const DXGI_FORMAT& { return format; }
   void SetFormat(const DXGI_FORMAT& format) { this->format = format; }

   auto GetBPP() const -> const uint& { return bpp; }
   void SetBPP(const uint& bpp) { this->bpp = bpp; }

   auto GetBPC() const -> const uint& { return bpc; }
   void SetBPC(const uint& bpc) { this->bpc = bpc; }

   auto GetWidth() const -> const uint& { return width; }
   void SetWidth(const uint& width) { this->width = width; }

   auto GetHeight() const -> const uint& { return height; }
   void SetHeight(const uint& height) { this->height = height; }

   auto GetChannels() const -> const uint& { return channels; }
   void SetChannels(const uint& channels) { this->channels = channels; }

   auto IsGrayscale() const -> const bool { return is_gray_scale; }
   void SetGrayscale(const bool& is_gray_scale) { this->is_gray_scale = is_gray_scale; }

   auto IsTransparent() const -> const bool { return is_transparent; }
   void SetTransparent(const bool& is_transparent) { this->is_transparent = is_transparent; }

   auto IsGenerateMipChain() const -> const bool { return is_generate_mip_map; }
   void SetGenerateMipChain(const bool& is_generate_mip_map) { this->is_generate_mip_map = is_generate_mip_map; }
   
   //=================================================================================================================
   //[Load]
   //=================================================================================================================
   auto Load_Foreign(const std::string& path, const bool& is_generate_mip_map) -> const bool; //확장자가 .PNG, .bmp일 때 사용
   auto Load_Native(const std::string& path) -> const bool;

   //=================================================================================================================
   //[MICS]
   //=================================================================================================================
   void GetMipChain(std::vector<mip_level>* mip_chain);
   void SetMipChain(const std::vector<mip_level>& mip_chain) {this->mip_chain=mip_chain;}
   void ClearMipChain();

   auto GetMipLevel(const uint& level) -> mip_level*;
   auto AddMipLevel() -> mip_level* {return &mip_chain.emplace_back(mip_level());}

   auto HasMipChain() -> const bool {return !mip_chain.empty();}

   auto GetByteCountFromMipChain() -> const uint;

   virtual const bool CreateGpuResource()=0;

protected:
    auto GetChannelsFromFormat(const DXGI_FORMAT& format) ->const uint;

protected:
 //D3D11에서 자원 뷰는 Device에서 생성 및 해제를 담당하기 때문에 필요
   ID3D11Device* device = nullptr;

   //bit per pixel (픽셀당 비트)
   uint bpp=0;
   //bit per channel (채널당 비트)
   uint bpc=8;
   uint width=0;
   uint height=0;
   uint channels=0;
   uint array_size=1;
   bool is_gray_scale=false;
   //투명
   bool is_transparent=false;
   bool is_generate_mip_map=true;

   DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM;
   TEXTURE_BIND_FLAGS bind_flags=TEXTURE_BIND_SRV;

   D3D11_VIEWPORT viewport;

   //mip_level이 작을 수록 원본을 의미
   //mip_chain은 렌더링 속도를 향상시키기 위한 목적으로 기본 텍스처와
   //이를 연속적으로 미리 축소시킨 텍스처들로 이루어진 비트맵 이미지의 집합.
   std::vector<mip_level> mip_chain;

   ID3D11ShaderResourceView* shader_resource_view=nullptr;
   ID3D11UnorderedAccessView* unordered_access_view= nullptr;
   ID3D11RenderTargetView* render_target_view= nullptr;
   std::vector<ID3D11DepthStencilView*> depth_stencil_views;
};