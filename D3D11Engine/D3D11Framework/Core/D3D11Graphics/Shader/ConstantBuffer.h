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
	auto Map()->T*; //CPU가 constant buffer에 접근하여 데이터를 쓰기위해 접근할 수 있도록 잠시 lock을 거는 함수
	void Unmap(); //CPU가 constant buffer에 접근 후 데이터를 사용하고 걸었던 lock을 푸는 함수 

private:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
	ID3D11Buffer* buffer = nullptr;
};

template<typename T>
inline void ConstantBuffer::Create(const D3D11_USAGE & usage)
{
	//버퍼를 정의(constant buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //버퍼의 접근에 관련
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //=contant buffer로 사용
	desc.ByteWidth = sizeof(T); //크기

	//버퍼의 접근에 관련된 flag
	switch (usage)
	{
	case D3D11_USAGE_DEFAULT:
	case D3D11_USAGE_IMMUTABLE: desc.CPUAccessFlags = 0; break;
	case D3D11_USAGE_DYNAMIC: desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; break;
	case D3D11_USAGE_STAGING: desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE; break;
	}

	//constant buffer는 다른 buffer들과 다르게 D3D11_SUBRESOURCE_DATA가 필요하지않음
	//고정되어 있는 실제 데이터가 아니고 계속 변경되는 값이기 때문
	auto hr = device->CreateBuffer(&desc, nullptr, &buffer);
	assert(SUCCEEDED(hr));
}

template<typename T>
inline auto ConstantBuffer::Map() -> T *
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource; //constant buffer에 값을 넣을 포인터 선언
	//constant buffer에 lock을 걺
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
