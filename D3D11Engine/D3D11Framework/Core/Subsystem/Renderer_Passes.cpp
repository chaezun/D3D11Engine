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
#include "Scene/Component/Animator.h"

void Renderer::PassMain()
{
	PassStandard();
	PassLine(render_textures[RenderTextureType::Final]);
}

void Renderer::PassStandard()
{
	command_list->Begin("PassMain");
	{
		auto& final_texture = render_textures[RenderTextureType::Final];

		if (renderables[RenderableType::Opaque].empty())
		{
			command_list->ClearRenderTarget(final_texture);
			command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH);
			command_list->End();
			command_list->Submit();
			return;
		}

		const auto& vertex_shader = shaders[ShaderType::VS_STANDARD];
		if (!vertex_shader)
		{
			command_list->End();
			command_list->Submit();
			return;
		}

		UpdateGlobalBuffer(final_texture->GetWidth(), final_texture->GetHeight());

		uint current_material_id = 0;
		uint current_mesh_id = 0;
		uint current_shader_id = 0;

		const auto draw_actor = [this, &current_material_id, &current_mesh_id, &current_shader_id](Actor* actor)
		{
			const auto& renderable = actor->GetRenderable();
			if (!renderable)
				return;

			const auto& material = renderable->GetMaterial();
			if (!material)
				return;

			const auto& pixel_shader = material->GetShader();
			if (!pixel_shader)
				return;

			const auto& mesh = renderable->GetMesh();
			if (!mesh || !mesh->GetVertexBuffer() || !mesh->GetIndexBuffer())
				return;

			if (current_mesh_id != mesh->GetID())
			{
				command_list->SetVertexBuffer(mesh->GetVertexBuffer());
				command_list->SetIndexBuffer(mesh->GetIndexBuffer());
				current_mesh_id = mesh->GetID();
			}

			if (current_shader_id != pixel_shader->GetID())
			{
				command_list->SetPixelShader(pixel_shader);
				current_shader_id = pixel_shader->GetID();
			}

			if (current_material_id != material->GetID())
			{
				command_list->SetRasterizerState(GetRasterizerState(material->GetCullMode()));
				command_list->SetShaderResources(0, ShaderScope::PS, material->GetTextureShaderResources());

				material->UpdateConstantBuffer();
				command_list->SetConstantBuffer(1, ShaderScope::PS, material->GetConstantBuffer());
				current_material_id = material->GetID();
			}

			/*if (skybox)
			{
				command_list->SetVertexShader(shaders[ShaderType::VPS_SKYBOX]);
				command_list->SetPixelShader(shaders[ShaderType::VPS_SKYBOX]);
			}*/

			const auto& transform = actor->GetTransform();
			transform->UpdateConstantBuffer(camera_view_proj);
			command_list->SetConstantBuffer(2, ShaderScope::VS, transform->GetConstantBuffer());

			command_list->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), mesh->GetIndexBuffer()->GetOffset(), mesh->GetVertexBuffer()->GetOffset());
		};

		command_list->SetRasterizerState(cull_back_solid_state);
		command_list->SetDepthStencilState(depth_stencil_enabled_state);
		command_list->SetRenderTarget(final_texture, depth_texture);
		command_list->SetViewport(final_texture->GetViewport());
		command_list->ClearRenderTarget(final_texture);
		command_list->ClearDepthStencilTarget(depth_texture, D3D11_CLEAR_DEPTH, 1.0f);
		command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command_list->SetInputLayout(vertex_shader->GetInputLayout());
		command_list->SetVertexShader(vertex_shader);
		command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
		command_list->SetSamplerState(0, ShaderScope::PS, anisotropic_wrap_sampler);

		if (!renderables[RenderableType::Directional_Light].empty())
		{
			const auto& light = renderables[RenderableType::Directional_Light].front()->GetComponent<Light>();

			light->UpdateConstantBuffer();
			command_list->SetConstantBuffer(3, ShaderScope::PS, light->GetConstantBuffer());
		}

		for (const auto& actor : renderables[RenderableType::Opaque])
			draw_actor(actor);
	}
	command_list->End();
	command_list->Submit();
}

void Renderer::PassLine(const std::shared_ptr<ITexture>& out)
{
	const bool is_draw_aabb = renderer_options & RendererOption_AABB;
	const bool is_draw_grid = renderer_options & RendererOption_Grid;
	const auto& shader = shaders[ShaderType::VPS_COLOR];
	if (!shader)
		return;

	command_list->Begin("PassLine");

	if (is_draw_aabb)
	{
		for (const auto& actor : renderables[RenderableType::Opaque])
		{
			if (auto renderable = actor->GetRenderable())
				DrawBox(renderable->GetBoundBox());
		}
	}

	command_list->SetViewport(out->GetViewport());
	command_list->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	command_list->SetInputLayout(shader->GetInputLayout());
	command_list->SetVertexShader(shader);
	command_list->SetPixelShader(shader);
	command_list->SetSamplerState(0, ShaderScope::PS, point_clamp_sampler);

	//Depth enabled
	command_list->SetDepthStencilState(depth_stencil_enabled_state);
	command_list->SetRenderTarget(out, depth_texture);
	{
		//Grid
		{
			if (is_draw_grid)
			{
				UpdateGlobalBuffer
				(
					static_cast<uint>(resolution.x),
					static_cast<uint>(resolution.y),
					grid->GetComputeWorldMatrix(camera->GetTransform()) * camera_view_proj
				);

				command_list->SetVertexBuffer(grid->GetVertexBuffer());
				command_list->SetIndexBuffer(grid->GetIndexBuffer());
				command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
				command_list->DrawIndexed(grid->GetIndexCount(), 0, 0);
			}
		}

		//Line
		{
			const auto vertex_count = static_cast<uint>(depth_enabled_line_vertices.size());
			if (vertex_count)
			{
				if (vertex_count > line_vertex_buffer->GetCount())
				{
					line_vertex_buffer->Clear();
					line_vertex_buffer->Create(depth_enabled_line_vertices, D3D11_USAGE_DYNAMIC);
				}

				auto vertex_data = static_cast<VertexColor*>(line_vertex_buffer->Map());
				std::copy(depth_enabled_line_vertices.begin(), depth_enabled_line_vertices.end(), vertex_data);
				line_vertex_buffer->Unmap();

				UpdateGlobalBuffer(out->GetWidth(), out->GetHeight(), camera_view_proj);

				command_list->SetVertexBuffer(line_vertex_buffer);
				command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
				command_list->Draw(vertex_count);

				depth_enabled_line_vertices.clear();
			}
		}
	}

	//Depth disabled
	command_list->SetDepthStencilState(depth_stencil_disabled_state);
	command_list->SetRenderTarget(out);
	{
		//Line
		{
			const auto vertex_count = static_cast<uint>(depth_disabled_line_vertices.size());
			if (vertex_count)
			{
				if (vertex_count > line_vertex_buffer->GetCount())
				{
					line_vertex_buffer->Clear();
					line_vertex_buffer->Create(depth_disabled_line_vertices, D3D11_USAGE_DYNAMIC);
				}

				auto vertex_data = static_cast<VertexColor*>(line_vertex_buffer->Map());
				std::copy(depth_disabled_line_vertices.begin(), depth_disabled_line_vertices.end(), vertex_data);
				line_vertex_buffer->Unmap();

				UpdateGlobalBuffer(out->GetWidth(), out->GetHeight(), camera_view_proj);

				command_list->SetVertexBuffer(line_vertex_buffer);
				command_list->SetConstantBuffer(0, ShaderScope::Global, global_buffer);
				command_list->Draw(vertex_count);

				depth_disabled_line_vertices.clear();
			}
		}
	}

	command_list->End();
	command_list->Submit();
}
