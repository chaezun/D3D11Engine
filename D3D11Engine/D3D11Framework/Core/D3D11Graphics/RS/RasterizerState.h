#pragma once

//RS�ܰ迡���� ���� -> ID3D11RasterizerState���� ����
//1. ������ �ʴ� ������Ƽ�� ����(��������, �ø� ó��)
//2. ��ǥ�� ����Ʈ ��ȯ
//3. ���� �׽�Ʈ
//4. ���� ���̾ ���
//5. ������Ƽ����� ���� Ÿ�� ���� �ؼ� ������ ����
//6. ��Ƽ ���ø��̳� �� ����� ������ ���� ó��

class RasterizerState final : public D3D11_Object
{
public:
	RasterizerState(class Context* context);
	~RasterizerState();

	auto GetResource() const ->ID3D11RasterizerState* { return state; }

	void Create(const D3D11_CULL_MODE& cull_mode, const D3D11_FILL_MODE& fill_mode);
	void Clear();

private:
   ID3D11Device* device			= nullptr;
   ID3D11RasterizerState* state = nullptr;

};