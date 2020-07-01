#pragma once

class Grid final
{
public:
	Grid(class Context* context);
	~Grid() = default;

	auto GetComputeWorldMatrix(class Transform* camera_transform) -> const Matrix&;
	auto GetVertexBuffer() const -> const std::shared_ptr<class VertexBuffer>& { return vertex_buffer; }
	auto GetIndexBuffer() const -> const std::shared_ptr<IndexBuffer>& { return index_buffer; }
	auto GetIndexCount() const -> const uint& { return index_count; }

private:
	void UpdateGrid
	(
		std::vector<struct VertexColor>& vertices,
		std::vector<uint>& indices
	);

	void CreateBuffers
	(
		const std::vector<struct VertexColor>& vertices,
		const std::vector<uint>& indices
	);

private:
	class Context* context = nullptr;
	Matrix world           = Matrix::Identity;
	uint index_count	   = 0;
	uint width             = 200;
	uint height            = 200;

	std::shared_ptr<class VertexBuffer> vertex_buffer;
	std::shared_ptr<class IndexBuffer> index_buffer;
};