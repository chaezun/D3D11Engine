#pragma once

//GPU���� Vertex������ ������ VertexBuffer�� �����Ҷ� ���� �޸� ����� �����ϱ� ������
//GPU���� Buffer�� ��Ȯ�� �Ӽ��� �����س��� ���ؼ��� Vertex Layout�� �˾ƾ���.
//�̸� ���ؼ� Input Layout�� ����ؾ� ��.

//Input Layout�� GPU���ٰ� Vertex�� ����ü�� ���̾ƿ��� �����ϴ� D3D Object��.

class InputLayout final : public D3D11_Object
{
public:
	InputLayout(class Context* context);
	~InputLayout();

	auto GetResource() const -> ID3D11InputLayout* { return input_layout; }

	//�����ϵ� HLSL�� ������ �ְ� �̸� ���ǵ� desc�� �ִ� ��쿡 ���
	void Create(D3D11_INPUT_ELEMENT_DESC* descs, const uint& count, ID3DBlob* blob);
	//�����ϵ� HLSL�� ������ �ְ� �̸� ���ǵ� desc�� ���� ��쿡 ���
	void Create(ID3DBlob* blob);
	void Clear();

private:
	ID3D11Device* device = nullptr;
	ID3D11InputLayout* input_layout = nullptr;
};