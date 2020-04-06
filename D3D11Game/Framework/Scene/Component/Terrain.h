#pragma once
#include "IComponent.h"

enum class TerrainMode : uint
{
	None,
	PaintTerrain,
	PaintTree,
	PaintDetail,
	Settings,
};

enum class PaintMode : uint
{
	PaintColor,
	PaintAlpha,
	PaintTexture,
	Raise,
	Lower,
	Flatten,
	Smooth,
};

enum class BrushType : uint
{
	None,
	Rectangle,
	Circle,
	Custom,
};

class Terrain final : public IComponent
{
public:
	Terrain
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Terrain() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//=====================================================================
	// [Terrain Edit]
	//=====================================================================
	auto GetTerrainMode() const -> const TerrainMode& { return terrain_mode; }
	void SetTerrainMode(const TerrainMode& mode) { this->terrain_mode = mode; }

	auto GetPaintMode() const -> const PaintMode& { return paint_mode; }
	void SetPaintMdoe(const PaintMode& mode) { this->paint_mode = mode; }

	auto GetBrushType() const -> const BrushType& { return brush_type; }
	void SetBrushType(const BrushType& type) { this->brush_type = type; }

	auto GetTerrainWidth() const -> const uint& { return width; }
	void SetTerrainWidth(const uint& width) { this->width = width; }

	auto GetTerrainHeight() const -> const uint& { return height; }
	void SetTerrainHeight(const uint& height) { this->height = height; }

	auto GetHeightWeight() const -> const float& { return height_weight; }
	void SetHeightWeight(const float& weight) { this->height_weight = weight; }

	auto GetBrushRange() const -> const float& { return brush_range; }
	void SetBrushRange(const float& range) { this->brush_range = range; }

	auto GetBrushColor() const -> const Color4& { return brush_color; }
	void SetBrushColor(const Color4& color) { this->brush_color = color; }

	auto GetTerrainColor() const -> const Color4& { return terrain_color; }
	void SetTerrainColor(const Color4& color) { this->terrain_color = color; }

	auto IsTerrainSelected() const -> const bool& { return is_terrain_selected; }
	void SetTerrainSelected(const bool& is_selected) { this->is_terrain_selected = is_selected; }

	auto GetSplatTextures() const -> const std::map<std::string, std::shared_ptr<Texture2D>>& { return splat_textures; }
	auto GetSplatTexture(const std::string& name) -> const std::shared_ptr<Texture2D>;
	void AddSplatTexture(const std::string& path);
	void AddSplatTexture(const std::shared_ptr<Texture2D>& texture);

	auto Pick(const Ray& ray, Vector3* position) -> const bool;

	//=====================================================================
	// [Terrain HeightMap]
	//=====================================================================
	auto GetHeightMap() const -> const std::shared_ptr<Texture2D>& { return height_map; }
	void SetHeightMap(const std::shared_ptr<Texture2D>& height_map) { this->height_map = height_map; }
	void SetHeightMap(const std::string& path);

	auto GetMinY() const -> const float& { return min_y; }
	void SetMinY(const float& min_y) { this->min_y = min_y; }

	auto GetMaxY() const -> const float& { return max_y; }
	void SetMaxY(const float& max_y) { this->max_y = max_y; }

	auto GetProgress() const -> const float;
	auto GetProgressState() const -> const std::string& { return progress_state; }

	auto HasHeightMap() const -> const bool { return height_map != nullptr; }

	void Generate();

	//=====================================================================
	// [Constant Buffer]
	//=====================================================================
	auto GetConstantBuffer() const -> const std::shared_ptr<class ConstantBuffer>& { return gpu_buffer; }
	void UpdateConstantBuffer();

	auto GetTextureShaderResources() -> const std::vector<ID3D11ShaderResourceView*>;

private:
	auto GeneratePosition(Vector3* positions, std::byte* height_map_datas) -> const bool;
	auto GenerateVerticesAndIndices(Vector3* positions) -> const bool;
	auto GenerateNormalAndTangent() -> const bool;

	void UpdateRenderable();

private:
	auto PaintColor(const D3D11_BOX& box) -> const bool;
	auto PaintTexture(const D3D11_BOX& box) -> const bool;
	auto Raise(const D3D11_BOX& box) -> const bool;
	auto Lower(const D3D11_BOX& box) -> const bool;
	auto Flatten(const D3D11_BOX& box) -> const bool;
	auto Smooth(const D3D11_BOX& box) -> const bool;

private:
	std::vector<VertexTextureNormalTangent> vertices;
	std::vector<uint> indices;
	std::shared_ptr<Texture2D> height_map;
	std::map<std::string, std::shared_ptr<Texture2D>> splat_textures;

	TerrainMode terrain_mode = TerrainMode::PaintTerrain;
	PaintMode paint_mode = PaintMode::Raise;
	BrushType brush_type = BrushType::Rectangle;
	uint width = 0;
	uint height = 0;
	float height_weight = 1.0f;
	float brush_range = 10.0f;
	Color4 brush_color = Color4::Green;
	Color4 terrain_color = Color4::Red;
	Vector3 brush_position = Vector3::Zero;
	float min_y = 0.0f;
	float max_y = 30.0f;
	float vertex_density = 1.0f;
	uint vertex_count = 0;
	uint face_count = 0;
	bool is_generated = false;
	bool is_terrain_selected = false;
	uint progress_jobs_done = 0;
	uint progress_job_count = 1;
	std::string progress_state = "";

	TERRAIN_DATA cpu_buffer;
	std::shared_ptr<class ConstantBuffer> gpu_buffer;
};