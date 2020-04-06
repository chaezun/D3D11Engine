#include "Framework.h"
#include "Terrain.h"
#include "Renderable.h"
#include "Camera.h"
#include "Scene/Actor.h"

Terrain::Terrain(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
	AddSplatTexture("../../../_Assets/Texture/Dirt.png");
}

void Terrain::OnStart()
{
}

void Terrain::OnUpdate()
{
    if(!is_terrain_selected)
	   return;

	if (terrain_mode == TerrainMode::PaintTerrain)
	{
		auto camera = context->GetSubsystem<Renderer>()->GetCamera();
		auto is_picked = Pick(camera->GetCameraRay(), &brush_position);

		if (is_picked && context->GetSubsystem<Input>()->BtnPress(KeyCode::CLICK_LEFT))
		{
		    auto half_width= static_cast<int>(width * 0.5f);
			auto half_height= static_cast<int>(height * 0.5f);
		    auto left = static_cast<int>(brush_position.x) - static_cast<int>(brush_range);
			auto right = static_cast<int>(brush_position.x) + static_cast<int>(brush_range);
			auto top = static_cast<int>(brush_position.z) + static_cast<int>(brush_range);
			auto bottom = static_cast<int>(brush_position.z) - static_cast<int>(brush_range);

		    D3D11_BOX box;
			box.left	=static_cast<uint>(left <=	 -half_width  ? -half_width + half_width : left + half_width);
			box.right   =static_cast<uint>(right >=	 +half_width  ? +half_width + half_width : right + half_width);
			box.top	    =static_cast<uint>(top >=	 +half_height ? +half_height + half_height  : top + half_height);
			box.bottom  =static_cast<uint>(bottom <=  -half_height ? -half_height + half_height : bottom + half_height);

			switch (paint_mode)
			{
			case PaintMode::PaintColor:  PaintColor(box);break;
			case PaintMode::PaintAlpha:
				break;
			case PaintMode::PaintTexture: PaintTexture(box); break;
			case PaintMode::Raise: Raise(box); break;
			case PaintMode::Lower: Lower(box); break;
			case PaintMode::Flatten: Flatten(box); break;
			case PaintMode::Smooth: Smooth(box); break;
			}
		}
	}
}

void Terrain::OnStop()
{
}

auto Terrain::GetSplatTexture(const std::string & name) -> const std::shared_ptr<Texture2D>
{
	if (splat_textures.find(name) == splat_textures.end())
		return nullptr;

	return splat_textures[name];
}

void Terrain::AddSplatTexture(const std::string & path)
{
	auto texture = context->GetSubsystem<ResourceManager>()->Load<Texture2D>(path);

	if (splat_textures.find(texture->GetResourceName()) != splat_textures.end())
		return;

	splat_textures[texture->GetResourceName()] = texture;
}

void Terrain::AddSplatTexture(const std::shared_ptr<Texture2D>& texture)
{
	if (splat_textures.find(texture->GetResourceName()) != splat_textures.end())
		return;

	splat_textures[texture->GetResourceName()] = texture;
}

auto Terrain::Pick(const Ray & ray, Vector3* position) -> const bool
{
	auto vertices_ptr = vertices.data();
	auto indices_ptr = indices.data();

	for (uint i = 0; i < indices.size(); i += 3)
	{
		auto index0 = indices_ptr[i + 0];
		auto index1 = indices_ptr[i + 1];
		auto index2 = indices_ptr[i + 2];

		auto v0 = vertices_ptr[index0].position;
		auto v1 = vertices_ptr[index1].position;
		auto v2 = vertices_ptr[index2].position;

		float t, u, v;

		if (Math::IntersectTriangle(ray.GetStart(), ray.GetDirection(), v0, v1, v2, &t, &u, &v))
		{
			if (position)
			  *position = ray.GetStart() + t * ray.GetDirection();

            

			

			return true;
		}
	}

	return false;
}

void Terrain::SetHeightMap(const std::string & path)
{
	height_map = context->GetSubsystem<ResourceManager>()->Load<Texture2D>(path);
}

auto Terrain::GetProgress() const -> const float
{
	return static_cast<float>(static_cast<double>(progress_jobs_done) / static_cast<double>(progress_job_count));
}

void Terrain::Generate()
{
	if (is_generated)
	{
		LOG_WARNING("Terrain is already being generated");
		return;
	}

	if (!height_map)
	{
		LOG_WARNING("You need to assign a height map before trying to generated a terrain");
		return;
	}

	context->GetSubsystem<Thread>()->AddTask([this]()
	{
		is_generated = true;

		auto height_map_data = height_map->GetMipLevel(0);
		if (!height_map_data)
			LOG_ERROR("Height map has no data");

		width = height_map->GetWidth();
		height = height_map->GetHeight();
		vertex_count = width * height;
		face_count = (height - 1) * (width - 1) * 2;
		progress_jobs_done = 0;
		progress_job_count = vertex_count * 2 + face_count + vertex_count * face_count;

		std::vector<Vector3> positions(vertex_count);

		vertices.clear();
		vertices.shrink_to_fit();
		vertices.reserve(vertex_count);
		vertices.resize(vertex_count);

		indices.clear();
		indices.shrink_to_fit();
		indices.reserve(face_count * 3);
		indices.resize(face_count * 3);

		progress_state = "Generating position...";
		if (GeneratePosition(positions.data(), height_map_data->data()))
		{
			progress_state = "Generating terrain vertices and indices...";
			if (GenerateVerticesAndIndices(positions.data()))
			{
				progress_state = "Generating normals and tangents...";
				positions.clear();
				positions.shrink_to_fit();

				if (GenerateNormalAndTangent())
					UpdateRenderable();
			}
		}

		progress_state.clear();
		progress_jobs_done = 0;
		progress_job_count = 1;

		is_generated = false;
	});
}

void Terrain::UpdateConstantBuffer()
{
	if (!gpu_buffer)
	{
		gpu_buffer = std::make_shared<ConstantBuffer>(context);
		gpu_buffer->Create<TERRAIN_DATA>();
	}

	auto is_update = false;
	is_update |= cpu_buffer.brush_position != brush_position;
	is_update |= cpu_buffer.brush_type != static_cast<int>(brush_type);
	is_update |= cpu_buffer.brush_color != brush_color;
	is_update |= cpu_buffer.brush_range != brush_range;

	if (!is_update)
		return;

	auto gpu_data = gpu_buffer->Map<TERRAIN_DATA>();
	if (!gpu_data)
	{
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_data->brush_position = cpu_buffer.brush_position = brush_position;
	gpu_data->brush_type = cpu_buffer.brush_type = static_cast<int>(brush_type);
	gpu_data->brush_color = cpu_buffer.brush_color = brush_color;
	gpu_data->brush_range = cpu_buffer.brush_range = brush_range;


	gpu_buffer->Unmap();
}

auto Terrain::GetTextureShaderResources() -> const std::vector<ID3D11ShaderResourceView *>
{
	std::vector<ID3D11ShaderResourceView*> shader_resources;

	for (const auto& texture : splat_textures)
	{
		shader_resources.emplace_back(texture.second->GetShaderResourceView());
	}

	return shader_resources;
}

auto Terrain::GeneratePosition(Vector3 * positions, std::byte * height_map_datas) -> const bool
{
	if (!height_map_datas)
	{
		LOG_ERROR("Height map is empty");
		return false;
	}

	uint byte = 0;
	for (uint z = 0; z < height; z++)
	{
		for (uint x = 0; x < width; x++)
		{
			auto y = static_cast<float>(height_map_datas[byte]) / 255.0f;

			uint index = z * width + x;

			positions[index].x = static_cast<float>(x) - width * 0.5f;
			positions[index].y = Math::Lerp(min_y, max_y, y);
			positions[index].z = static_cast<float>(z) - height * 0.5f;

			byte += 4;
			progress_jobs_done++;
		}
	}

	return true;
}

auto Terrain::GenerateVerticesAndIndices(Vector3 * positions) -> const bool
{
	if (!positions)
	{
		LOG_ERROR("Positions are empty");
		return false;
	}

	auto vertices_ptr = vertices.data();
	auto indices_ptr = indices.data();
	uint index = 0;
	uint k = 0;
	uint u_offset = 0;
	uint v_offset = 0;

	for (uint z = 0; z < height - 1; z++)
	{
		for (uint x = 0; x < width - 1; x++)
		{
			uint bottom_left = z * width + x;
			uint bottom_right = z * width + (x + 1);
			uint top_left = (z + 1) * width + x;
			uint top_right = (z + 1) * width + (x + 1);

			//Bottom Left
			index = bottom_left;
			indices_ptr[k] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 0.0f, v_offset + 1.0f));

			//Top Left
			index = top_left;
			indices_ptr[k + 1] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 0.0f, v_offset + 0.0f));

			//Bottom Right
			index = bottom_right;
			indices_ptr[k + 2] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 1.0f, v_offset + 1.0f));

			//Bottom Right
			index = bottom_right;
			indices_ptr[k + 3] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 1.0f, v_offset + 1.0f));

			//Top Left
			index = top_left;
			indices_ptr[k + 4] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 0.0f, v_offset + 0.0f));

			//Top Right
			index = top_right;
			indices_ptr[k + 5] = index;
			vertices_ptr[index] = VertexTextureNormalTangent(positions[index], Vector2(u_offset + 1.0f, v_offset + 0.0f));

			k += 6;
			u_offset++;
			progress_jobs_done++;
		}
		u_offset = 0;
		v_offset++;
	}

	return true;
}

auto Terrain::GenerateNormalAndTangent() -> const bool
{
	if (vertices.empty())
	{
		LOG_ERROR("Vertices are empty");
		return false;
	}

	if (indices.empty())
	{
		LOG_ERROR("Indices are empty");
		return false;
	}

	std::vector<Vector3> face_normals(face_count);

	auto vertices_ptr = vertices.data();
	auto indices_ptr = indices.data();

	for (uint i = 0; i < face_count; i++)
	{
		auto index0 = indices_ptr[i * 3 + 0];
		auto index1 = indices_ptr[i * 3 + 1];
		auto index2 = indices_ptr[i * 3 + 2];

		auto e1 = Vector3(vertices_ptr[index0].position - vertices_ptr[index1].position);
		auto e2 = Vector3(vertices_ptr[index1].position - vertices_ptr[index2].position);
		face_normals[i] = Vector3::Cross(e1, e2);

		progress_jobs_done++;
	}

	const auto compute_normal_tangent = [this, &face_normals, &vertices_ptr, &indices_ptr](const uint& start, const uint& end)
	{
		Vector3 normal_sum = Vector3::Zero;
		float faces_using = 0.0f;

		for (uint i = start; i < end; i++)
		{
			for (uint j = 0; j < face_count; j++)
			{
				if (indices_ptr[j * 3 + 0] == i || indices_ptr[j * 3 + 1] == i || indices_ptr[j * 3 + 2] == i)
				{
					normal_sum += face_normals[j];
					faces_using++;
				}

				if (j == face_count < 1)
					progress_jobs_done += face_count;
			}

			//Compute Normal
			normal_sum /= faces_using;
			normal_sum.Normalize();

			vertices_ptr[i].normal = normal_sum;

			normal_sum = Vector3::Zero;
			faces_using = 0.0f;
		}
	};

	context->GetSubsystem<Thread>()->Loop(compute_normal_tangent, vertex_count);

	return true;
}

void Terrain::UpdateRenderable()
{
	if (vertices.empty() || indices.empty())
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	if (auto& renderable = actor->AddComponent<Renderable>())
	{
		renderable->SetMesh("Terrain", vertices, indices, D3D11_USAGE_DYNAMIC);
		renderable->SetStandardMaterial();
	}
}

auto Terrain::PaintColor(const D3D11_BOX & box) -> const bool
{
	auto vertices_ptr = vertices.data();

	for (uint z = box.bottom; z < box.top; z++)
	{
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices_ptr[index].terrain_color = terrain_color;
		}
	}

	auto renderable = actor->GetRenderable();
	auto mesh = renderable ? renderable->GetMesh() : nullptr;
	auto vertex_buffer = mesh ? mesh->GetVertexBuffer() : nullptr;

	if (!vertex_buffer)
	{
		LOG_ERROR("VertexBuffer is nullptr");
		return false;
	}

	auto vertex_data = static_cast<VertexTextureNormalTangent*>(vertex_buffer->Map());
	std::copy(vertices.begin(), vertices.end(), vertex_data);
	vertex_buffer->Unmap();

	return true;
}

auto Terrain::PaintTexture(const D3D11_BOX & box) -> const bool
{
	auto vertices_ptr = vertices.data();

	for (uint z = box.bottom; z < box.top; z++)
	{
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices_ptr[index].splat_color.a = 1.0f;
		}
	}

	auto renderable = actor->GetRenderable();
	auto mesh = renderable ? renderable->GetMesh() : nullptr;
	auto vertex_buffer = mesh ? mesh->GetVertexBuffer() : nullptr;

	if (!vertex_buffer)
	{
		LOG_ERROR("VertexBuffer is nullptr");
		return false;
	}

	auto vertex_data = static_cast<VertexTextureNormalTangent*>(vertex_buffer->Map());
	std::copy(vertices.begin(), vertices.end(), vertex_data);
	vertex_buffer->Unmap();

	return true;
}

auto Terrain::Raise(const D3D11_BOX & box) -> const bool
{ 
    auto vertices_ptr = vertices.data();

	for (uint z = box.bottom; z < box.top; z++)
	{
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices_ptr[index].position.y += height_weight;
		}
	}

	//여기서 프레임드랍 발생
	//GenerateNormalAndTangent();

	auto renderable = actor->GetRenderable();
	auto mesh = renderable ? renderable->GetMesh() : nullptr;
	auto vertex_buffer = mesh ? mesh->GetVertexBuffer() : nullptr;  

	if (!vertex_buffer)
	{
		LOG_ERROR("VertexBuffer is nullptr");
		return false;
	}

	auto vertex_data = static_cast<VertexTextureNormalTangent*>(vertex_buffer->Map());
	std::copy(vertices.begin(), vertices.end(), vertex_data);
	vertex_buffer->Unmap();

	return true;
}

auto Terrain::Lower(const D3D11_BOX & box) -> const bool
{
	auto vertices_ptr = vertices.data();

	for (uint z = box.bottom; z < box.top; z++)
	{
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices_ptr[index].position.y -= height_weight;
		}
	}

	auto renderable = actor->GetRenderable();
	auto mesh = renderable ? renderable->GetMesh() : nullptr;
	auto vertex_buffer = mesh ? mesh->GetVertexBuffer() : nullptr;

	if (!vertex_buffer)
	{
		LOG_ERROR("VertexBuffer is nullptr");
		return false;
	}

	auto vertex_data = static_cast<VertexTextureNormalTangent*>(vertex_buffer->Map());
	std::copy(vertices.begin(), vertices.end(), vertex_data);
	vertex_buffer->Unmap();

	return true;
}

auto Terrain::Flatten(const D3D11_BOX & box) -> const bool
{
	return false;
}

auto Terrain::Smooth(const D3D11_BOX & box) -> const bool
{
	return false;
}
