#pragma once
#include "D3D11Framework.h"

//Config의 사용용도 : 렌더링에 필요한 데이터를 미리 정의하고 사용.

static const std::string NOT_ASSIGNED_STR = "N/A";
static const std::string MODEL_BIN_EXTENSION = ".model";
static const std::string MESH_BIN_EXTENSION = ".mesh";
static const std::string TEXTURE_BIN_EXTENSION = ".texture";
static const std::string MATERIAL_BIN_EXTENSION = ".material";

enum class ShaderScope : uint
{
    Unknown, 
	Global, //Constant Buffer  
	VS,     //Vertex Shader
	PS      //Pixel Shader
};

struct GLOBAL_DATA final
{
	Matrix world_view_proj;
	Matrix view;
	Matrix proj;
	Matrix view_proj;
	Matrix view_proj_inverse;
	Matrix post_process_proj;
	Matrix post_process_view_proj;
	float camera_near;
	float camera_far;
	Vector2 resolution;
	Vector3 camera_position;
	float padding;
};

struct TRANSFORM_DATA final
{
    Matrix world;
};

struct MATERIAL_DATA final
{
	Color4 albedo_color;
	Vector2 uv_tiling;
	Vector2 uv_offset;
	float roughness_coef;
	float metallic_coef;
	float normal_coef;
	float height_coef;
	float shading_mode;
	float padding[3];
};

struct LIGHT_DATA final
{
	Vector3 color;
	float range; 
	Vector3 position;
	float intensity;
	Vector3 direction;
	float angle_radian;
	float bias;
	float normal_bias;
	float padding[2];
};

struct TERRAIN_DATA final
{
	Vector3 brush_position;
	int brush_type;
	Vector3 brush_color;
	float brush_range;
};