#include "D3D11Framework.h"
#include "Grid.h"
#include "Scene/Component/Transform.h"

Grid::Grid(Context * context)
	:context(context)
{
	std::vector<VertexColor> vertices;
	std::vector<uint> indices;

	UpdateGrid(vertices, indices);
	CreateBuffers(vertices, indices);
}

auto Grid::GetComputeWorldMatrix(Transform * camera_transform) -> const Matrix &
{
	const auto grid_spacing = 1.0f;
	const auto translation = Vector3
	(
		static_cast<int>(camera_transform->GetTranslation().x / grid_spacing) * grid_spacing,
		0.0f,
		static_cast<int>(camera_transform->GetTranslation().z / grid_spacing) * grid_spacing
	);

	auto S = Matrix::Scaling(grid_spacing);
	auto T = Matrix::Translation(translation);

	world = S * T;

	return world;
}

void Grid::UpdateGrid(std::vector<struct VertexColor>& vertices, std::vector<uint>& indices)
{
	const auto half_width = static_cast<int>(width * 0.5f);
	const auto half_height = static_cast<int>(height * 0.5f);

	for (auto j = -half_height; j < half_height; j++)
	{
		for (auto i = -half_width; i < half_width; i++)
		{
			const auto alpha_width = 1.0f - static_cast<float>(Math::Abs(i)) / static_cast<float>(half_width);
			const auto alpha_height = 1.0f - static_cast<float>(Math::Abs(j)) / static_cast<float>(half_height);

			auto alpha = (alpha_width + alpha_height) * 0.5f;
			alpha = powf(alpha, 10.0f);

			//Line1
			//-Left Top
			auto position_x = static_cast<float>(i);
			auto position_z = static_cast<float>(j + 1);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//-Right Top
			position_x = static_cast<float>(i + 1);
			position_z = static_cast<float>(j + 1);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//Line2
			//-Right Top
			position_x = static_cast<float>(i + 1);
			position_z = static_cast<float>(j + 1);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//-Right Bottom
			position_x = static_cast<float>(i + 1);
			position_z = static_cast<float>(j);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//Line3
			//-Right Bottom
			position_x = static_cast<float>(i + 1);
			position_z = static_cast<float>(j);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//-Left Bottom
			position_x = static_cast<float>(i);
			position_z = static_cast<float>(j);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//Line4
			//-Left Bottom
			position_x = static_cast<float>(i);
			position_z = static_cast<float>(j);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));

			//-Left Top
			position_x = static_cast<float>(i);
			position_z = static_cast<float>(j + 1);
			vertices.emplace_back(Vector3(position_x, 0.0f, position_z), Color4(1.0f, 1.0f, 1.0f, alpha));
		}
	}

	for (uint i = 0; i < static_cast<uint>(vertices.size()); i++)
		indices.emplace_back(i);

	index_count = static_cast<uint>(indices.size());
}

void Grid::CreateBuffers(const std::vector<struct VertexColor>& vertices, const std::vector<uint>& indices)
{
	vertex_buffer = std::make_shared<VertexBuffer>(context);
	vertex_buffer->Create(vertices);

	index_buffer = std::make_shared<IndexBuffer>(context);
	index_buffer->Create(indices);



}
