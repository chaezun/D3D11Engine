#pragma once

//GPU에게 Vertex정보를 보관한 VertexBuffer를 전송할때 보통 메모리 덩어리로 전송하기 때문에
//GPU에서 Buffer의 정확한 속성을 추출해내기 위해서는 Vertex Layout을 알아야함.
//이를 위해서 Input Layout을 사용해야 함.

//Input Layout은 GPU에다가 Vertex의 구조체의 레이아웃을 설명하는 D3D Object임.

class InputLayout final : public D3D11_Object
{
public:
	InputLayout(class Context* context);
	~InputLayout();

	auto GetResource() const -> ID3D11InputLayout* { return input_layout; }

	//컴파일된 HLSL의 정보가 있고 미리 정의된 desc가 있는 경우에 사용
	void Create(D3D11_INPUT_ELEMENT_DESC* descs, const uint& count, ID3DBlob* blob);
	//컴파일된 HLSL의 정보가 있고 미리 정의된 desc가 없는 경우에 사용
	void Create(ID3DBlob* blob);
	void Clear();

private:
	ID3D11Device* device = nullptr;
	ID3D11InputLayout* input_layout = nullptr;
};