#pragma once

class ConstantBuffer final : public D3D11_Object
{
public:
	ConstantBuffer(class Context* context);
	~ConstantBuffer();

	auto GetResource() const->ID3D11Buffer* { return buffer; }

	template <typename T>
	void Create(const D3D11_USAGE& usage = D3D11_USAGE_DYNAMIC);
	void Clear();

	template <typename T>
	auto Map()->T*; //CPU�� constant buffer�� �����Ͽ� �����͸� �������� ������ �� �ֵ��� ��� lock�� �Ŵ� �Լ�
	void Unmap(); //CPU�� constant buffer�� ���� �� �����͸� ����ϰ� �ɾ��� lock�� Ǫ�� �Լ� 

private:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
	ID3D11Buffer* buffer = nullptr;
};

template<typename T>
inline void ConstantBuffer::Create(const D3D11_USAGE & usage)
{
	//���۸� ����(constant buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //������ ���ٿ� ����
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //=contant buffer�� ���
	desc.ByteWidth = sizeof(T); //ũ��

	//������ ���ٿ� ���õ� flag
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT:
	case D3D11_USAGE_IMMUTABLE: desc.CPUAccessFlags = 0; break;
	case D3D11_USAGE_DYNAMIC: desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; break;
	case D3D11_USAGE_STAGING: desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE; break;
	}

	//constant buffer�� �ٸ� buffer��� �ٸ��� D3D11_SUBRESOURCE_DATA�� �ʿ���������
	//�����Ǿ� �ִ� ���� �����Ͱ� �ƴϰ� ��� ����Ǵ� ���̱� ����
	auto hr = device->CreateBuffer(&desc, nullptr, &buffer);
	assert(SUCCEEDED(hr));
}

template<typename T>
inline auto ConstantBuffer::Map() -> T *
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource; //constant buffer�� ���� ���� ������ ����
	//constant buffer�� lock�� ��
	auto hr = device_context->Map
	(
		buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_resource
	);

	assert(SUCCEEDED(hr));

	return reinterpret_cast<T*>(mapped_resource.pData);
}
