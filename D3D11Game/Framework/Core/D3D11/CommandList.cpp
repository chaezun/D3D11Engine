#include "Framework.h"
#include "CommandList.h"

Command::Command()
{
	constant_buffers.reserve(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	shader_resources.reserve(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	sampler_states.reserve(D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	render_targets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	Clear();
}

void Command::Clear()
{
	command_type = CommandType::Unknown;
	pass_name = NOT_ASSIGNED_STR;

	vertex_buffer = nullptr;
	vertex_stride = 0;
	vertex_offset = 0;
	vertex_count = 0;

	index_buffer = nullptr;
	index_offset = 0;
	index_count = 0;

	input_layout = nullptr;
	primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vertex_shader = nullptr;
	pixel_shader = nullptr;

	constant_buffers.clear();
	constant_buffer_shader_scope = ShaderScope::Unknown;
	constant_buffer_slot = 0;
	constant_buffer_count = 0;

	shader_resources.clear();
	shader_resource_shader_scope = ShaderScope::Unknown;
	shader_resource_slot = 0;
	shader_resource_count = 0;

	sampler_states.clear();
	sampler_state_shader_scope = ShaderScope::Unknown;
	sampler_state_slot = 0;
	sampler_state_count = 0;

	rasterizer_state = nullptr;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	blend_state = nullptr;
	depth_stencil_state = nullptr;

	render_targets.clear();
	render_target_count = 0;

	depth_stencil_target = nullptr;

	clear_render_target = nullptr;
	clear_color = Color4::Zero;

	clear_depth_stencil_target = nullptr;
	clear_flags = 0;
	clear_stencil = 0;
	clear_depth = 0.0f;
}

CommandList::CommandList(Context * context)
{
	device_context = context->GetSubsystem<Graphics>()->GetDeviceContext();

	commands.reserve(command_capacity);
	commands.resize(command_capacity);
}

CommandList::~CommandList()
{
	Clear();
}

void CommandList::Begin(const std::string & pass_name)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::Begin;
	cmd.pass_name = pass_name;
}

void CommandList::End()
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::End;
}

void CommandList::Draw(const uint & vertex_count)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::Draw;
	cmd.vertex_count = vertex_count;
}

void CommandList::DrawIndexed(const uint & index_count, const uint & index_offset, const uint & vertex_offset)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::DrawIndexed;
	cmd.index_count = index_count;
	cmd.index_offset = index_offset;
	cmd.vertex_offset = vertex_offset;
}

void CommandList::SetVertexBuffer(VertexBuffer * buffer)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetVertexBuffer;
	cmd.vertex_buffer = buffer ? buffer->GetResource() : nullptr;
	cmd.vertex_count = buffer ? buffer->GetCount() : 0;
	cmd.vertex_stride = buffer ? buffer->GetStride() : 0;
	cmd.vertex_offset = buffer ? buffer->GetOffset() : 0;
}

void CommandList::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
{
	SetVertexBuffer(buffer.get());
}

void CommandList::SetIndexBuffer(IndexBuffer * buffer)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetIndexBuffer;
	cmd.index_buffer = buffer ? buffer->GetResource() : nullptr;
	cmd.index_count = buffer ? buffer->GetCount() : 0;
	cmd.index_offset = buffer ? buffer->GetOffset() : 0;
}

void CommandList::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer)
{
	SetIndexBuffer(buffer.get());
}

void CommandList::SetInputLayout(InputLayout * input_layout)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetInputLayout;
	cmd.input_layout = input_layout ? input_layout->GetResource() : nullptr;
}

void CommandList::SetInputLayout(const std::shared_ptr<InputLayout>& input_layout)
{
	SetInputLayout(input_layout.get());
}

void CommandList::SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY & primitive_topology)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetPrimitiveTopology;
	cmd.primitive_topology = primitive_topology;
}

void CommandList::SetVertexShader(Shader * shader)
{
	SetVertexShader(shader->GetShader<VertexShader>());
}

void CommandList::SetVertexShader(const std::shared_ptr<Shader>& shader)
{
	SetVertexShader(shader.get());
}

void CommandList::SetVertexShader(VertexShader * shader)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetVertexShader;
	cmd.vertex_shader = shader ? shader->GetResource() : nullptr;
}

void CommandList::SetVertexShader(const std::shared_ptr<VertexShader>& shader)
{
	SetVertexShader(shader.get());
}

void CommandList::SetPixelShader(Shader * shader)
{
	SetPixelShader(shader->GetShader<PixelShader>());
}

void CommandList::SetPixelShader(const std::shared_ptr<Shader>& shader)
{
	SetPixelShader(shader.get());
}

void CommandList::SetPixelShader(PixelShader * shader)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetPixelShader;
	cmd.pixel_shader = shader ? shader->GetResource() : nullptr;
}

void CommandList::SetPixelShader(const std::shared_ptr<PixelShader>& shader)
{
	SetPixelShader(shader.get());
}


void CommandList::SetConstantBuffer(const uint & slot, const ShaderScope & scope, ConstantBuffer * buffer)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetConstantBuffer;
	cmd.constant_buffer_slot = slot;
	cmd.constant_buffer_shader_scope = scope;
	cmd.constant_buffer_count++;
	cmd.constant_buffers.emplace_back(buffer->GetResource());
}

void CommandList::SetConstantBuffer(const uint & slot, const ShaderScope & scope, const std::shared_ptr<ConstantBuffer>& buffer)
{
	SetConstantBuffer(slot, scope, buffer.get());
}

void CommandList::SetConstantBuffers(const uint & slot, const ShaderScope & scope, const std::vector<ID3D11Buffer*>& buffers)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetConstantBuffer;
	cmd.constant_buffer_slot = slot;
	cmd.constant_buffer_shader_scope = scope;
	cmd.constant_buffers = buffers;
	cmd.constant_buffer_count = static_cast<uint>(buffers.size());
}

void CommandList::SetShaderResource(const uint & slot, const ShaderScope & scope, ITexture * texture)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetShaderResource;
	cmd.shader_resource_slot = slot;
	cmd.shader_resource_shader_scope = scope;
	cmd.shader_resource_count++;
	cmd.shader_resources.emplace_back(texture ? texture->GetShaderResourceView() : nullptr);
}

void CommandList::SetShaderResource(const uint & slot, const ShaderScope & scope, const std::shared_ptr<ITexture>& texture)
{
	SetShaderResource(slot, scope, texture.get());
}

void CommandList::SetShaderResources(const uint & slot, const ShaderScope & stage, const std::vector<ID3D11ShaderResourceView*>& shader_resources)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetShaderResource;
	cmd.shader_resource_slot = slot;
	cmd.shader_resource_shader_scope = stage;
	cmd.shader_resource_count = static_cast<uint>(shader_resources.size());
	cmd.shader_resources = shader_resources;
}

void CommandList::SetSamplerState(const uint & slot, const ShaderScope & scope, SamplerState * sampler_state)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetSamplerState;
	cmd.sampler_state_slot = slot;
	cmd.sampler_state_shader_scope = scope;
	cmd.sampler_state_count++;
	cmd.sampler_states.emplace_back(sampler_state ? sampler_state->GetResource() : nullptr);
}

void CommandList::SetSamplerState(const uint & slot, const ShaderScope & stage, const std::shared_ptr<SamplerState>& sampler_state)
{
	SetSamplerState(slot, stage, sampler_state.get());
}

void CommandList::SetSamplerStates(const uint & slot, const ShaderScope & stage, const std::vector<ID3D11SamplerState*>& sampler_states)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetSamplerState;
	cmd.sampler_state_slot = slot;
	cmd.sampler_state_shader_scope = stage;
	cmd.sampler_state_count = static_cast<uint>(sampler_states.size());
	cmd.sampler_states = sampler_states;
}

void CommandList::SetViewport(const D3D11_VIEWPORT & viewport)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetViewport;
	cmd.viewport = viewport;
}

void CommandList::SetRasterizerState(RasterizerState * rasterizer_state)
{
    auto& cmd = GetCommand();
    cmd.command_type = CommandType::SetRasterizerState;
    cmd.rasterizer_state = rasterizer_state ? rasterizer_state->GetResource() : nullptr;
}

void CommandList::SetRasterizerState(const std::shared_ptr<RasterizerState>& rasterizer_state)
{
    SetRasterizerState(rasterizer_state.get());
}

//void CommandList::SetBlendState(BlendState * blend_state)
//{
//    auto& cmd = GetCommand();
//    cmd.command_type = CommandType::SetBlendState;
//    cmd.blend_state = blend_state ? blend_state->GetResource() : nullptr;
//}
//
//void CommandList::SetBlendState(const std::shared_ptr<BlendState>& blend_state)
//{
//    SetBlendState(blend_state.get());
//}

void CommandList::SetDepthStencilState(DepthStencilState * depth_stencil_state)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetDepthStencilState;
	cmd.depth_stencil_state = depth_stencil_state ? depth_stencil_state->GetResource() : nullptr;
}

void CommandList::SetDepthStencilState(const std::shared_ptr<DepthStencilState>& depth_stencil_state)
{
	SetDepthStencilState(depth_stencil_state.get());
}

void CommandList::SetRenderTarget(ITexture * render_target, ITexture * depth_stencil_target)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetRenderTarget;
	cmd.depth_stencil_target = depth_stencil_target ? depth_stencil_target->GetDepthStencilView() : nullptr;
	cmd.render_targets.emplace_back(render_target ? render_target->GetRenderTargetView() : nullptr);
	cmd.render_target_count++;
}

void CommandList::SetRenderTarget(const std::shared_ptr<ITexture>& render_target, const std::shared_ptr<ITexture>& depth_stencil_target)
{
	SetRenderTarget(render_target.get(), depth_stencil_target.get());
}

void CommandList::SetRenderTargets(const std::vector<ID3D11RenderTargetView*>& render_targets, ID3D11DepthStencilView * depth_stencil_target)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::SetRenderTarget;
	cmd.depth_stencil_target = depth_stencil_target;
	cmd.render_targets = render_targets;
	cmd.render_target_count = static_cast<uint>(render_targets.size());
}

void CommandList::ClearRenderTarget(ITexture * render_target, const Color4 & clear_color)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::ClearRenderTarget;
	cmd.clear_render_target = render_target ? render_target->GetRenderTargetView() : nullptr;
	cmd.clear_color = clear_color;
}

void CommandList::ClearRenderTarget(const std::shared_ptr<ITexture>& render_target, const Color4 & clear_color)
{
	ClearRenderTarget(render_target.get(), clear_color);
}

void CommandList::ClearRenderTargets(const std::vector<ID3D11RenderTargetView*>& render_targets, const Color4 & clear_color)
{
	for (const auto& render_target : render_targets)
	{
		auto& cmd = GetCommand();
		cmd.command_type = CommandType::ClearRenderTarget;
		cmd.clear_render_target = render_target;
		cmd.clear_color = clear_color;
	}
}

void CommandList::ClearDepthStencilTarget(ITexture * depth_stencil_target, const D3D11_CLEAR_FLAG & clear_flag, const float & clear_depth, const uint & clear_stencil)
{
	auto& cmd = GetCommand();
	cmd.command_type = CommandType::ClearDepthStencilTarget;
	cmd.clear_depth_stencil_target = depth_stencil_target ? depth_stencil_target->GetDepthStencilView() : nullptr;
	cmd.clear_flags = clear_flag;
	cmd.clear_depth = clear_depth;
	cmd.clear_stencil = clear_stencil;
}

void CommandList::ClearDepthStencilTarget(const std::shared_ptr<ITexture>& depth_stencil_target, const D3D11_CLEAR_FLAG & clear_flag, const float & clear_depth, const uint & clear_stencil)
{
	ClearDepthStencilTarget(depth_stencil_target.get(), clear_flag, clear_depth, clear_stencil);
}

void CommandList::ClearShaderResources(const ShaderScope & scope)
{
	auto empty_textures = std::vector<ID3D11ShaderResourceView*>(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	SetShaderResources(0, scope, empty_textures);
}

auto CommandList::Submit() -> const bool
{
	for (uint i = 0; i < command_count; i++)
	{
		auto& cmd = commands[i];

		switch (cmd.command_type)
		{
		case CommandType::Begin:
			break;
		case CommandType::End:
			break;
		case CommandType::Draw:
			device_context->Draw(cmd.vertex_count, 0);
			break;
		case CommandType::DrawIndexed:
			device_context->DrawIndexed(cmd.index_count, cmd.index_offset, cmd.vertex_offset);
			break;
		case CommandType::SetVertexBuffer:
			device_context->IASetVertexBuffers(0, 1, &cmd.vertex_buffer, &cmd.vertex_stride, &cmd.vertex_offset);
			break;
		case CommandType::SetIndexBuffer:
			device_context->IASetIndexBuffer(cmd.index_buffer, DXGI_FORMAT_R32_UINT, cmd.index_offset);
			break;
		case CommandType::SetInputLayout:
			device_context->IASetInputLayout(cmd.input_layout);
			break;
		case CommandType::SetPrimitiveTopology:
			device_context->IASetPrimitiveTopology(cmd.primitive_topology);
			break;
		case CommandType::SetVertexShader:
			device_context->VSSetShader(cmd.vertex_shader, nullptr, 0);
			break;
		case CommandType::SetPixelShader:
			device_context->PSSetShader(cmd.pixel_shader, nullptr, 0);
			break;
		case CommandType::SetConstantBuffer:
		{
			switch (cmd.constant_buffer_shader_scope)
			{
			case ShaderScope::Global:
			case ShaderScope::VS: device_context->VSSetConstantBuffers(cmd.constant_buffer_slot, cmd.constant_buffer_count, cmd.constant_buffers.data()); if (cmd.constant_buffer_shader_scope != ShaderScope::Global) break;
			case ShaderScope::PS: device_context->PSSetConstantBuffers(cmd.constant_buffer_slot, cmd.constant_buffer_count, cmd.constant_buffers.data()); if (cmd.constant_buffer_shader_scope != ShaderScope::Global) break;
			}
			break;
		}
		case CommandType::SetShaderResource:
		{
			switch (cmd.shader_resource_shader_scope)
			{
			case ShaderScope::Global:
			case ShaderScope::VS: device_context->VSSetShaderResources(cmd.shader_resource_slot, cmd.shader_resource_count, cmd.shader_resources.data()); if (cmd.shader_resource_shader_scope != ShaderScope::Global) break;
			case ShaderScope::PS: device_context->PSSetShaderResources(cmd.shader_resource_slot, cmd.shader_resource_count, cmd.shader_resources.data()); if (cmd.shader_resource_shader_scope != ShaderScope::Global) break;
			}
			break;
		}
		case CommandType::SetSamplerState:
		{
			switch (cmd.sampler_state_shader_scope)
			{
			case ShaderScope::Global:
			case ShaderScope::VS: device_context->VSSetSamplers(cmd.sampler_state_slot, cmd.sampler_state_count, cmd.sampler_states.data()); if (cmd.sampler_state_shader_scope != ShaderScope::Global) break;
			case ShaderScope::PS: device_context->PSSetSamplers(cmd.sampler_state_slot, cmd.sampler_state_count, cmd.sampler_states.data()); if (cmd.sampler_state_shader_scope != ShaderScope::Global) break;
			}
			break;
		}
		case CommandType::SetRasterizerState:
			device_context->RSSetState(cmd.rasterizer_state);
			break;
		case CommandType::SetViewport:
			device_context->RSSetViewports(1, &cmd.viewport);
			break;
		case CommandType::SetBlendState:
		{
			float blend_factor[]{ 0.0f, 0.0f, 0.0f, 0.0f };
			device_context->OMSetBlendState(cmd.blend_state, blend_factor, 0xffffffff);
			break;
		}
		case CommandType::SetDepthStencilState:
			device_context->OMSetDepthStencilState(cmd.depth_stencil_state, 1);
			break;
		case CommandType::SetRenderTarget:
			device_context->OMSetRenderTargets(cmd.render_target_count, cmd.render_targets.data(), cmd.depth_stencil_target);
			break;
		case CommandType::ClearRenderTarget:
			device_context->ClearRenderTargetView(cmd.clear_render_target, cmd.clear_color);
			break;
		case CommandType::ClearDepthStencilTarget:
			device_context->ClearDepthStencilView(cmd.clear_depth_stencil_target, cmd.clear_flags, cmd.clear_depth, cmd.clear_stencil);
			break;
		}
	}

	Clear();
	return true;
}

auto CommandList::GetCommand() -> Command &
{
	if (command_count >= static_cast<uint>(commands.size()))
	{
		uint new_size = command_count + 100;
		commands.reserve(new_size);
		commands.resize(new_size);

		//TODO : LOG
	}

	return commands[command_count++];
}

void CommandList::Clear()
{
	for (uint i = 0; i < command_count; i++)
		commands[i].Clear();

	command_count = 0;
}