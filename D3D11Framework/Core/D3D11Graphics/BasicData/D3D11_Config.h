#pragma once
#include "D3D11Framework.h"

//Config의 사용용도 : 렌더링에 필요한 데이터를 미리 정의하고 사용.

static const std::string NOT_ASSIGNED_STR			= "N/A";
static const std::string MODEL_BIN_EXTENSION		= ".model";
static const std::string MESH_BIN_EXTENSION			= ".mesh";
static const std::string TEXTURE_BIN_EXTENSION		= ".texture";
static const std::string MATERIAL_BIN_EXTENSION		= ".material";

enum class ShaderScope : uint
{
    Unknown, 
	Global, //Constant Buffer  
	VS,     //Vertex Shader
	PS      //Pixel Shader
};

//ConstantBuffer에 넘겨줄 구조체 변수들
//=================================================================================================================

//World Conversion
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

//Transform
struct TRANSFORM_DATA final
{
    Matrix world;
	Matrix wvp_current;
	Matrix wvp_previous;
};

//Material
struct MATERIAL_DATA final
{
	Color4 albedo_color;
	Vector2 uv_tiling;
	Vector2 uv_offset;
	float roughness_coef;
	float metallic_coef;
	float normal_coef;
	float shading_mode;
};

//Ligth
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

//Animation
#define MAX_BONE_COUNT 100
struct ANIMATION_DATA final
{
	Matrix skinned_transforms[MAX_BONE_COUNT];
};

//=================================================================================================================