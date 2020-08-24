#include "D3D11Framework.h"
#include "Renderer.h"
#include "Core/D3D11Graphics/CommandList.h"
#include "Gizmo/Grid.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/SkyBox.h"

Renderer::Renderer(Context * context)
	:ISubsystem(context)
{
    renderer_options |= 
	   RendererOption_AABB |
		RendererOption_Grid;

	SUBSCRIBE_TO_EVENT(EventType::Render, EVENT_HANDLER(Render));
}

Renderer::~Renderer()
{
}

const bool Renderer::Initialize()
{
    line_vertex_buffer = std::make_shared<VertexBuffer>(context);
	grid = std::make_shared<Grid>(context);
	command_list = std::make_shared<CommandList>(context);

	CreateRenderTextures();
	CreateShaders();
	CreateConstantBuffers();
	CreateSamplerStates();
	CreateRasterizerStates();
	CreateDepthStencilStates();

	return true;
}

auto Renderer::GetFrameResource() -> ID3D11ShaderResourceView *
{
	auto render_texture = render_textures[RenderTextureType::Final];

	return render_texture ? render_texture->GetShaderResourceView() : nullptr;
}

void Renderer::SetResolution(const uint & width, const uint & height)
{
	if (width == 0 || height == 0)
	{
		LOG_WARNING_F("%dx%d is an invalid resolution", width, height);
		return;
	}

	auto adjust_width = (width % 2 != 0) ? width - 1 : width;
	auto adjust_height = (height % 2 != 0) ? height - 1 : height;

	if (static_cast<uint>(resolution.x) == adjust_width && static_cast<uint>(resolution.y) == adjust_height)
		return;

	resolution.x = static_cast<float>(adjust_width);
	resolution.y = static_cast<float>(adjust_height);

	CreateRenderTextures();

	LOG_INFO_F("Resolution set to %dx%d", width, height);
}

void Renderer::AcquireRenderables(Scene * scene)
{
	renderables.clear();
	camera = nullptr;

	auto actors = scene->GetAllActors();
	for (const auto& actor : actors)
	{
		if (!actor)
			continue;
		//
        auto skybox_component = actor->GetComponent<SkyBox>();
		auto renderable_component = actor->GetComponent<Renderable>();
		auto light_component = actor->GetComponent<Light>();
		auto camera_component = actor->GetComponent<Camera>();

		//
		if (skybox_component)
		{ 
		   skybox = skybox_component;

		}

		if (renderable_component)
		{
			renderables[RenderableType::Opaque].emplace_back(actor.get());
		}

		if (light_component)
		{
			renderables[RenderableType::Light].emplace_back(actor.get());

			switch (light_component->GetLightType())
			{
			case LightType::Directional: renderables[RenderableType::Directional_Light].emplace_back(actor.get());  break;
			case LightType::Point:       renderables[RenderableType::Point_Light].emplace_back(actor.get());        break;
			case LightType::Spot:        renderables[RenderableType::Spot_Light].emplace_back(actor.get());         break;
			}
		}

		if (camera_component)
		{
			renderables[RenderableType::Camera].emplace_back(actor.get());
			camera = camera_component;
		}
	}
}

void Renderer::SortRenderables(std::vector<class Actor*>* actors)
{
}

void Renderer::DrawLine(const Vector3 & from, const Vector3 & to, const Color4 & from_color, const Color4 & to_color, const bool & is_depth)
{
	if (is_depth)
	{
		depth_enabled_line_vertices.emplace_back(from, from_color);
		depth_enabled_line_vertices.emplace_back(to, to_color);
	}

	else
	{
		depth_disabled_line_vertices.emplace_back(from, from_color);
		depth_disabled_line_vertices.emplace_back(to, to_color);
	}
}

void Renderer::DrawBox(const BoundBox & box, const Color4 & color, const bool & is_depth)
{
	const auto& min = box.GetMin();
	const auto& max = box.GetMax();

	//직육면체 모서리선을 그림
	DrawLine(Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z), color, color, is_depth);
	DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color, color, is_depth);
	DrawLine(Vector3(max.x, max.y, min.z), Vector3(min.x, max.y, min.z), color, color, is_depth);
	DrawLine(Vector3(min.x, max.y, min.z), Vector3(min.x, min.y, min.z), color, color, is_depth);
	DrawLine(Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), color, color, is_depth);
	DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), color, color, is_depth);
	DrawLine(Vector3(max.x, max.y, min.z), Vector3(max.x, max.y, max.z), color, color, is_depth);
	DrawLine(Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), color, color, is_depth);
	DrawLine(Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), color, color, is_depth);
	DrawLine(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), color, color, is_depth);
	DrawLine(Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z), color, color, is_depth);
	DrawLine(Vector3(min.x, max.y, max.z), Vector3(min.x, min.y, max.z), color, color, is_depth);
}

void Renderer::Render()
{
	if (!camera)
	{
		command_list->ClearRenderTarget(render_textures[RenderTextureType::Final], Color4::Black);
		return;
	}

	if (renderables.empty())
	{
		command_list->ClearRenderTarget(render_textures[RenderTextureType::Final], Color4::Black);
		return;
	}

	camera_near = camera->GetNearPlane();
	camera_far = camera->GetFarPlane();
	camera_position = camera->GetTransform()->GetTranslation();
	camera_view = camera->GetViewMatrix();
	camera_proj = camera->GetProjectionMatrix();
	camera_view_proj = camera_view * camera_proj;
	camera_view_proj_inverse = camera_view_proj.Inverse();

	post_process_view = Matrix::LookAtLH(Vector3(0.0f, 0.0f, -camera_near), Vector3::Forward, Vector3::Up);
	post_process_proj = Matrix::OrthoLH(resolution.x, resolution.y, camera_near, camera_far);
	post_process_view_proj = post_process_view * post_process_proj;


	PassMain();
}

void Renderer::UpdateGlobalBuffer(const uint & width, const uint & height, const Matrix & transform)
{
	auto gpu_buffer = global_buffer->Map<GLOBAL_DATA>();
	if (!gpu_buffer)
	{
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_buffer->world_view_proj = transform;
	gpu_buffer->view = camera_view;
	gpu_buffer->proj = camera_proj;
	gpu_buffer->view_proj = camera_view_proj;
	gpu_buffer->view_proj_inverse = camera_view_proj_inverse;
	gpu_buffer->post_process_proj = post_process_proj;
	gpu_buffer->post_process_view_proj = post_process_view_proj;

	gpu_buffer->camera_near = camera_near;
	gpu_buffer->camera_far = camera_far;
	gpu_buffer->resolution = Vector2(static_cast<float>(width), static_cast<float>(height));
	gpu_buffer->camera_position = camera_position;

	global_buffer->Unmap();
}

auto Renderer::GetRasterizerState(const D3D11_CULL_MODE & cull_mode, const D3D11_FILL_MODE & fill_mode) -> const std::shared_ptr<class RasterizerState>&
{
	switch (cull_mode)
	{
	case D3D11_CULL_NONE:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME: return cull_none_wireframe_state;
		case D3D11_FILL_SOLID: return cull_none_solid_state;
		}
		break;
	}
	case D3D11_CULL_FRONT:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME: return cull_front_wireframe_state;
		case D3D11_FILL_SOLID: return cull_front_solid_state;
		}
		break;
	}
	case D3D11_CULL_BACK:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME: return cull_back_wireframe_state;
		case D3D11_FILL_SOLID: return cull_back_solid_state;
		}
		break;
	}
	}

	return cull_back_solid_state;
}
