#pragma once
#include "ISubsystem.h"

class Graphics final : public ISubsystem
{
public:
    Graphics(class Context* context);
    ~Graphics();

    const bool Initialize() override;

    auto GetDevice() const -> ID3D11Device* { return device; }
    auto GetDeviceContext() const -> ID3D11DeviceContext* { return device_context; }

	void Resize(const uint& width, const uint& height);
	void SetViewport(const uint& width, const uint& height);

    void BeginScene();
    void EndScene();

private:
    void CreateSwapChain();
	void CreateRenderTargetView();
	void DeleteSurface();

private:
	//ID�� ������ com �������̽���
	//DX���� �ڿ��� ����(���⼭ �ڿ��� GPU�� �ڿ���)
	ID3D11Device* device = nullptr;
	//Device���� ���� �ڿ��� GPU�� ��� ������� �����ϴ� ����
	//����� �����ϴ� ����
	ID3D11DeviceContext* device_context = nullptr;

	//DX�� ������ �ִ� �⺻ �Ϻα���
	//IDXGISwapChain�� �������̽��� ȭ���� ����ϱ� ��
	//������ �� �����͸� ������
	IDXGISwapChain* swap_chain = nullptr;

	//����� �ϴ� ��ȭ�� ����
	ID3D11RenderTargetView* render_target_view = nullptr;
	//�������� ����
	D3D11_VIEWPORT viewport;
    Color4 clear_color=Color4(0xff555566);
	uint gpu_memory_size=0;
	uint numerator=0;
	uint denominator=0;
	std::wstring gpu_description=L"";
};