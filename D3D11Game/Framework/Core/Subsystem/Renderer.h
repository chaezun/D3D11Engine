#pragma once
#include "ISubsystem.h"

enum RendererOption : uint
{
	RendererOption_AABB = 1U << 0,
	RendererOption_Grid = 1U << 1,
};

enum class ShaderType : uint
{
	VS_STANDARD,
	PS_STANDARD,
	VPS_COLOR,

	//
	VPS_SKYBOX,
};

enum class RenderableType : uint
{
	Opaque,
	Transparent,
	Camera,
	Light,
	Directional_Light,
	Spot_Light,
	Point_Light,
};

enum class RenderTextureType : uint
{
	Final,
};

class Renderer final : public ISubsystem
{
public:
	Renderer(class Context* context);
	~Renderer();

	const bool Initialize() override;

	auto GetCamera() -> class Camera* { return camera.get(); }
	auto GetFrameResource()->ID3D11ShaderResourceView*;

	auto GetEditorOffset() const -> const Vector2& { return editor_offset; }
	void SetEditorOffset(const Vector2& offset) { this->editor_offset = offset; }

	auto GetResolution() const -> const Vector2& { return resolution; }
	void SetResolution(const uint& width, const uint& height);

	void AcquireRenderables(class Scene* scene);
	void SortRenderables(std::vector<class Actor*>* actors);

	void DrawLine
	(
		const Vector3& from,
		const Vector3& to,
		const Color4& from_color = Color4(0.41f, 0.86f, 1.0f, 1.0f),
		const Color4& to_color = Color4(0.41f, 0.86f, 1.0f, 1.0f),
		const bool& is_depth = true
	);

	void DrawBox
	(
	   const BoundBox& box, 
	   const Color4& color = Color4(0.41f, 0.86f, 1.0f, 1.0f),
	   const bool& is_depth = true
	);

	void Render();

	void FlagEnable(const RendererOption& flag)  { renderer_options |= flag; }
	void FlagDisable(const RendererOption& flag) { renderer_options &= ~flag; }
	auto IsOnFlag(const RendererOption& flag) -> const bool { return (renderer_options & flag) > 0U; }

private:
	void CreateRenderTextures();
	void CreateShaders();
	void CreateConstantBuffers();
	void CreateSamplerStates();
	void CreateRasterizerStates();
	void CreateDepthStencilStates();

	void UpdateGlobalBuffer(const uint& width, const uint& height, const Matrix& transform = Matrix::Identity);
	
	auto GetRasterizerState(const D3D11_CULL_MODE& cull_mode, const D3D11_FILL_MODE& fill_mode = D3D11_FILL_SOLID) -> const std::shared_ptr<class RasterizerState>&;
private:
	void PassMain();
	void PassStandard();
	void PassLine(const std::shared_ptr<class ITexture>& out);

private:
	uint renderer_options = 0;

	Matrix post_process_view;
	Matrix post_process_proj;
	Matrix post_process_view_proj;
	Matrix camera_view;
	Matrix camera_proj;
	Matrix camera_view_proj;
	Matrix camera_view_proj_inverse;
	float camera_near = 0.0f;
	float camera_far = 0.0f;
	Vector3 camera_position = Vector3::Zero;
	Vector2 editor_offset = Vector2::Zero;
	Vector2 resolution = Vector2(1280, 720);

	std::shared_ptr<class Camera> camera;
	std::shared_ptr<class CommandList> command_list;

private:
	std::vector<struct VertexColor> depth_enabled_line_vertices;
	std::vector<struct VertexColor> depth_disabled_line_vertices;
	std::shared_ptr<class VertexBuffer> line_vertex_buffer;
	std::shared_ptr<class Grid> grid;

	//
	std::shared_ptr<class SkyBox> skybox;

private:
	//Shaders
	std::map<ShaderType, std::shared_ptr<class Shader>> shaders;

	//Constant Buffers
	std::shared_ptr<class ConstantBuffer> global_buffer;

	//Sampler States
	std::shared_ptr<class SamplerState> point_clamp_sampler;
	std::shared_ptr<class SamplerState> bilinear_clamp_sampler;
	std::shared_ptr<class SamplerState> bilinear_wrap_sampler;
	std::shared_ptr<class SamplerState> trilinear_clamp_sampler;
	std::shared_ptr<class SamplerState> anisotropic_wrap_sampler;

	//Rasterizer States
	std::shared_ptr<class RasterizerState> cull_back_solid_state;
	std::shared_ptr<class RasterizerState> cull_front_solid_state;
	std::shared_ptr<class RasterizerState> cull_none_solid_state;
	std::shared_ptr<class RasterizerState> cull_back_wireframe_state;
	std::shared_ptr<class RasterizerState> cull_front_wireframe_state;
	std::shared_ptr<class RasterizerState> cull_none_wireframe_state;

	//Depth Stencil States
	std::shared_ptr<class DepthStencilState> depth_stencil_enabled_state;
	std::shared_ptr<class DepthStencilState> depth_stencil_disabled_state;

	//Render Textures
	std::map<RenderTextureType, std::shared_ptr<class ITexture>> render_textures;
	std::shared_ptr<class ITexture> depth_texture; //Test


	//Renderables
	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;
};

//RTT - Render To Texture
//화면에 그려질 놈을 저장하는 기법 (백버퍼는 그려질 놈을 화면에 띄우는 역할을 하는데 백버퍼와는 의미가 다름)