#include "Framework.h"
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

	CreateSwapChain();
	Resize
	(
		static_cast<uint>(Settings::Get().GetWidth()),
		static_cast<uint>(Settings::Get().GetHeight())
	);


	return true;
}

void Graphics::Resize(const uint & width, const uint & height)
{
	//Delete RTV
	DeleteSurface();

	//Resize
	{
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

	//Create RTV
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

void Graphics::BeginScene()
{
	device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	device_context->RSSetViewports(1, &viewport);
	device_context->ClearRenderTargetView(render_target_view, clear_color);
}

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
	desc.BufferDesc.RefreshRate.Numerator = Settings::Get().IsVsync() ? numerator : 0;
	desc.BufferDesc.RefreshRate.Denominator = Settings::Get().IsVsync() ? denominator : 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.OutputWindow = Settings::Get().GetWindowHandle();
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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

