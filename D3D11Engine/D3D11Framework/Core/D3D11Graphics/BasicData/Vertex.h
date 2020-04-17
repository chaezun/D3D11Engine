#pragma once

struct VertexColor final
{
	VertexColor()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
	{}

	VertexColor(const Vector3& position, const Color4& color)
		: position(position)
		, color(color)
	{}

	Vector3 position;
	Color4 color;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 2;
};

struct VertexTexture final
{
	VertexTexture()
		: position(0, 0, 0)
		, uv(0, 0)
	{}

	VertexTexture(const Vector3& position, const Vector2& uv)
		: position(position)
		, uv(uv)
	{}

	Vector3 position;
	Vector2 uv;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 2;
};

struct VertexTextureNormalTangent final
{
	VertexTextureNormalTangent()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, tangent(0, 0, 0)
		, indices(0.0f)
		, weights(0.0f)
	{}

	VertexTextureNormalTangent(const Vector3& position, const Vector2& uv)
		: position(position)
		, uv(uv)
		, normal(0.0f)
		, tangent(0.0f)
		, indices(0.0f)
		, weights(0.0f)
	{}

	VertexTextureNormalTangent(const Vector3& position, const Vector2& uv, const Vector3& normal, const Vector3& tangent, const Vector4& indices, const Vector4& weights)
		: position(position)
		, uv(uv)
		, normal(normal)
		, tangent(tangent)
		, indices(0.0f)
		, weights(0.0f)
	{}

	Vector3 position;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
	Vector4 indices;
	Vector4 weights;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 6;
};