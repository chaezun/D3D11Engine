#include "D3D11Framework.h"
#include "Graphics.h"

Graphics::Graphics(Context* context)
	: ISubsystem(context)
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
}

Graphics::~Graphics()
{
	SAFE_RELEASE(render_target_view);
	SAFE_RELEASE(device_context);
	SAFE_RELEASE(device);
	SAFE_RELEASE(swap_chain);
}

const bool Graphics::Initialize()
{
	//지원하는 해상도 찾기
	IDXGIFactory* factory = nullptr;
	auto hr = CreateDXGIFactory
	(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&factory)
	);
	assert(SUCCEEDED(hr));

	IDXGIAdapter* adapter = nullptr;
	hr = factory->EnumAdapters(0, &adapter);
	assert(SUCCEEDED(hr));

	IDXGIOutput* adapter_output = nullptr;
	hr = adapter->EnumOutputs(0, &adapter_output);
	assert(SUCCEEDED(hr));

	uint display_mode_count = 0;
	hr = adapter_output->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&display_mode_count,
		nullptr
	);
	assert(SUCCEEDED(hr));

	auto display_mode_list = new DXGI_MODE_DESC[display_mode_count];
	hr = adapter_output->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&display_mode_count,
		display_mode_list
	);
	assert(SUCCEEDED(hr));

	for (uint i = 0; i < display_mode_count; i++)
	{
		bool is_check = true;
		is_check &= display_mode_list[i].Width == static_cast<uint>(Settings::Get().GetWidth());
		is_check &= display_mode_list[i].Height == static_cast<uint>(Settings::Get().GetHeight());

		if (is_check)
		{
			numerator = display_mode_list[i].RefreshRate.Numerator;
			denominator = display_mode_list[i].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC adapter_desc;
	hr = adapter->GetDesc(&adapter_desc);

	gpu_memory_size = static_cast<uint>(adapter_desc.DedicatedVideoMemory / 1024 / 1024);
	gpu_description = adapter_desc.Description;

	SAFE_DELETE_ARRAY(display_mode_list);
	SAFE_RELEASE(adapter_output);
	SAFE_RELEASE(adapter);
	SAFE_RELEASE(factory);

	//=========================================================================================
	std::cout << "Numerator : " << numerator << std::endl;
	std::cout << "Denominator : " << denominator << std::endl;
	std::cout << "GPU Memory Size : " << gpu_memory_size << std::endl;
	std::wcout << "GPU Description : " << gpu_description << std::endl;
	//=========================================================================================

	CreateSwapChain(); //SwapChain 생성
	CreateRenderTargetView(); //RenderTargetView 생성
	Resize
	(
		static_cast<uint>(Settings::Get().GetWidth()),
		static_cast<uint>(Settings::Get().GetHeight())
	);


	return true;
}

void Graphics::Resize(const uint & width, const uint & height)
{
	DeleteSurface(); //RenderTargetView Release

	//Resize
	{
		//변경할 값을 제외한 값은 모두 0
		//출력을 할 화면의 크기를 설정
		//swap_chain의 버퍼를 다시 조정
		auto hr = swap_chain->ResizeBuffers
		(
			0,
			width,
			height,
			DXGI_FORMAT_UNKNOWN,
			0
		);
		assert(SUCCEEDED(hr));
	}

	CreateRenderTargetView();

	SetViewport(width, height);
}

void Graphics::SetViewport(const uint & width, const uint & height)
{
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

}

//새로 그리는 곳
void Graphics::BeginScene()
{
	device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	device_context->RSSetViewports(1, &viewport);
	device_context->ClearRenderTargetView(render_target_view, clear_color);
}

//그린 것을 앞으로 출력하는 곳
void Graphics::EndScene()
{
	auto hr = swap_chain->Present(Settings::Get().IsVsync() ? 1 : 0, 0);
	assert(SUCCEEDED(hr));
}

void Graphics::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	//주사율(프레임) 설정
	//분자
	desc.BufferDesc.RefreshRate.Numerator = Settings::Get().IsVsync() ? numerator : 0;
	desc.BufferDesc.RefreshRate.Denominator = Settings::Get().IsVsync() ? denominator : 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//Sample:특정 데이터를 추출
	//현재는 사용하지 않음
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.OutputWindow = Settings::Get().GetWindowHandle();
	desc.Windowed = TRUE;
	//SwapChain을 할때 완료한 데이터를 저장하지 않고 삭제
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//SSAA: 이미지를 몇배 확대시켜 계단현상을 조절
	//MSAA: 외곽선 부분만 계단현상을 조절
	//두 기법모두 비용이 많이 듦

	//D3D 버전 설정
	std::vector<D3D_FEATURE_LEVEL> feature_levels
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	//D3D_DRIVER_TYPE_HARDWARE: 컴퓨터에 내장된 그래픽카드를 그대로 사용한다는 뜻
	auto hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		feature_levels.data(),
		1,
		D3D11_SDK_VERSION,
		&desc,
		&swap_chain,
		&device,
		nullptr,
		&device_context
	);

	assert(SUCCEEDED(hr));
}

void Graphics::CreateRenderTargetView()
{
	ID3D11Texture2D* back_buffer = nullptr;
	//__uuidof : 범용고유 식별자 
	auto hr = swap_chain->GetBuffer
	(
		0,
		IID_ID3D11Texture2D,//__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&back_buffer)
	);
	assert(SUCCEEDED(hr));

	hr = device->CreateRenderTargetView
	(
		back_buffer,
		nullptr,
		&render_target_view
	);
	assert(SUCCEEDED(hr));


	SAFE_RELEASE(back_buffer);
}

void Graphics::DeleteSurface()
{
	SAFE_RELEASE(render_target_view);
}

