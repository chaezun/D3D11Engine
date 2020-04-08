#pragma once
#include "ISubsystem.h"

class Graphics final : public ISubsystem
{
public:
    Graphics(class Context* context);
    ~Graphics();

    const bool Initialize() override;

    auto GetDevice() const -> ID3D11Device* { return device; }
    auto GetDeviceContext() const -> ID3D11DeviceContext* { return device_context; }

	void Resize(const uint& width, const uint& height);
	void SetViewport(const uint& width, const uint& height);

    void BeginScene();
    void EndScene();

private:
    void CreateSwapChain();
	void CreateRenderTargetView();
	void DeleteSurface();

private:
	//ID가 붙으면 com 인터페이스임
	//DX에서 자원을 만듦(여기서 자원은 GPU의 자원임)
	ID3D11Device* device = nullptr;
	//Device에서 만든 자원을 GPU에 어떻게 사용할지 전달하는 역할
	//명령을 전달하는 역할
	ID3D11DeviceContext* device_context = nullptr;

	//DX가 가지고 있는 기본 하부구조
	//IDXGISwapChain의 인터페이스는 화면을 출력하기 전
	//렌더링 된 데이터를 저장함
	IDXGISwapChain* swap_chain = nullptr;

	//출력을 하는 도화지 역할
	ID3D11RenderTargetView* render_target_view = nullptr;
	//보여지는 영역
	D3D11_VIEWPORT viewport;
    Color4 clear_color=Color4(0xff555566);
	uint gpu_memory_size=0;
	uint numerator=0;
	uint denominator=0;
	std::wstring gpu_description=L"";
};