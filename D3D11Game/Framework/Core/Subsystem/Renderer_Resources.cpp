#include "Framework.h"
#include "Renderer.h"

void Renderer::CreateRenderTextures()
{
	auto width = static_cast<uint>(resolution.x);
	auto height = static_cast<uint>(resolution.y);

	if ((width / 4) == 0 || (height / 4) == 0)
	{
		LOG_WARNING_F("%dx%d is an invalid resolution", width, height);
		return;
	}

	//Final
	render_textures[RenderTextureType::Final] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, TEXTURE_BIND_RTV | TEXTURE_BIND_SRV);

	//Test
	depth_texture = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_D32_FLOAT, 1, TEXTURE_BIND_DSV | TEXTURE_BIND_SRV);
}

void Renderer::CreateShaders()
{
	const auto shader_directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Shader);

	//Vertex Shader
	auto vs_standard = std::make_shared<Shader>(context);
	vs_standard->AddShader<VertexShader>(shader_directory + "Prev/" + "Standard.hlsl");
	shaders[ShaderType::VS_STANDARD] = vs_standard;

	//Pixel Shader
	auto ps_standard = std::make_shared<Shader>(context);
	ps_standard->AddShader<PixelShader>(shader_directory + "Prev/"+ "Standard.hlsl");
	shaders[ShaderType::PS_STANDARD] = ps_standard;

	//Vertex Pixel Shader
	auto vps_color = std::make_shared<Shader>(context);
	vps_color->AddShader<VertexShader>(shader_directory + "Prev/" + "Color.hlsl");
	vps_color->AddShader<PixelShader>(shader_directory + "Prev/" + "Color.hlsl");
	shaders[ShaderType::VPS_COLOR] = vps_color;

	//
	auto vps_skybox = std::make_shared<Shader>(context);
	vps_skybox->AddShader<VertexShader>(shader_directory + "Prev/" + "SkyBox.hlsl");
	vps_skybox->AddShader<PixelShader>(shader_directory + "Prev/" + "SkyBox.hlsl");
	shaders[ShaderType::VPS_SKYBOX] = vps_skybox;
}

void Renderer::CreateConstantBuffers()
{
	global_buffer = std::make_shared<ConstantBuffer>(context);
	global_buffer->Create<GLOBAL_DATA>();
}

void Renderer::CreateSamplerStates()
{
	point_clamp_sampler = std::make_shared<SamplerState>(context);
	point_clamp_sampler->Create
	(
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	bilinear_clamp_sampler = std::make_shared<SamplerState>(context);
	bilinear_clamp_sampler->Create
	(
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	bilinear_wrap_sampler = std::make_shared<SamplerState>(context);
	bilinear_wrap_sampler->Create
	(
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS
	);

	trilinear_clamp_sampler = std::make_shared<SamplerState>(context);
	trilinear_clamp_sampler->Create
	(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	anisotropic_wrap_sampler = std::make_shared<SamplerState>(context);
	anisotropic_wrap_sampler->Create
	(
		D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS
	);
}

void Renderer::CreateRasterizerStates()
{
	cull_back_solid_state = std::make_shared<RasterizerState>(context);
	cull_back_solid_state->Create(D3D11_CULL_BACK, D3D11_FILL_SOLID);

	cull_front_solid_state = std::make_shared<RasterizerState>(context);
	cull_front_solid_state->Create(D3D11_CULL_FRONT, D3D11_FILL_SOLID);

	cull_none_solid_state = std::make_shared<RasterizerState>(context);
	cull_none_solid_state->Create(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	cull_back_wireframe_state = std::make_shared<RasterizerState>(context);
	cull_back_wireframe_state->Create(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);

	cull_front_wireframe_state = std::make_shared<RasterizerState>(context);
	cull_front_wireframe_state->Create(D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME);

	cull_none_wireframe_state = std::make_shared<RasterizerState>(context);
	cull_none_wireframe_state->Create(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);

}

void Renderer::CreateDepthStencilStates()
{
	depth_stencil_enabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_enabled_state->Create(true, D3D11_COMPARISON_LESS_EQUAL);

	depth_stencil_disabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_disabled_state->Create(false, D3D11_COMPARISON_LESS_EQUAL);
}