#include "Framework.h"
#include "Geometry_Generator.h"

void Geometry_Generator::CreateQuad(Geometry<struct VertexTextureNormalTangent>& geometry)
{
   geometry.AddVertex(VertexTextureNormalTangent(Vector3(-0.5f,-0.5f,0.0f),Vector2(0.0f,1.0f),Vector3(0.0f,0.0f,0.0f), Vector3(0.0f, 0.0f, 0.0f)));
   geometry.AddVertex(VertexTextureNormalTangent(Vector3(-0.5f, 0.5f, 0.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f)));
   geometry.AddVertex(VertexTextureNormalTangent(Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f)));
   geometry.AddVertex(VertexTextureNormalTangent(Vector3(+0.5f, +0.5f, 0.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f)));

   geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
   geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);
}

void Geometry_Generator::CreateSphere(Geometry<struct VertexTextureNormalTangent>& geometry, const float & radius, const int & slices, const int & stacks)
{
	auto normal = Vector3(0, 1, 0);
	auto tangent = Vector3(1, 0, 0);

	geometry.AddVertex(VertexTextureNormalTangent(Vector3(0, radius, 0), Vector2(0, 0), normal, tangent));

	float phiStep = static_cast<float>(Math::PI / stacks);
	float thetaStep = static_cast<float>(Math::PI * 2.0 / slices);

	for (int i = 1; i <= stacks - 1; i++)
	{
		float phi = i * phiStep;

		for (int j = 0; j <= slices; j++)
		{
			float theta = j * thetaStep;

			Vector3 position = Vector3
			(
				radius * sin(phi) * cos(theta),
				radius * cos(phi),
				radius * sin(phi) * sin(theta)
			);

			Vector2 uv = Vector2
			(
				static_cast<float>(theta / (Math::PI * 2.0)),
				static_cast<float>(phi / Math::PI)
			);

			Vector3 t = Vector3(-radius * sin(phi) * sin(theta), 0, radius * sin(phi) * cos(theta));
			t.Normalize();

			Vector3 n = position;
			n.Normalize();

			geometry.AddVertex(VertexTextureNormalTangent(position, uv, normal, tangent));
		}
	}

	normal = Vector3(0, -1, 0);
	tangent = Vector3(1, 0, 0);

	geometry.AddVertex(VertexTextureNormalTangent(Vector3(0, -radius, 0), Vector2(0, 1), normal, tangent));;

	for (int i = 1; i <= slices; i++)
	{
		geometry.AddIndex(0);
		geometry.AddIndex(i + 1);
		geometry.AddIndex(i);
	}

	int baseIndex = 1;
	int ringVertexCount = slices + 1;

	for (int i = 0; i < stacks - 2; i++)
		for (int j = 0; j < slices; j++)
		{
			geometry.AddIndex(baseIndex + i * ringVertexCount + j);
			geometry.AddIndex(baseIndex + i * ringVertexCount + j + 1);
			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j);

			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
			geometry.AddIndex(baseIndex + i * ringVertexCount + j + 1);
			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}

	int southPoleIndex = static_cast<int>(geometry.GetVertexCount() - 1);
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i < slices; i++)
	{
		geometry.AddIndex(southPoleIndex);
		geometry.AddIndex(baseIndex + i);
		geometry.AddIndex(baseIndex + i + 1);
	}
}

