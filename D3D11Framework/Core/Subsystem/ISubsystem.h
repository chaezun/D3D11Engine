#pragma once

//�߻�Ŭ���� - �Ϲ� Ŭ������ �������� �Լ��� �ϳ� �̻� ���Ե� �� -> �߻�Ŭ������ ���� �ν��Ͻ��� �� ����
//C++������ �������̽��� ���� ���� ������ �߻�Ŭ������ ������ �̿��Ͽ�
//����� �κ��� ���� �����Լ��� ����� ��ӹ޴� Ŭ�������� �����ϵ��� ��.
//�� ������Ʈ������ I��� ���λ簡 ������ �������̽��� ������ ��.
class ISubsystem
{
public:
    ISubsystem(class Context* context)
        : context(context)
    {}
    virtual ~ISubsystem() = default;

    virtual const bool Initialize() = 0;

    auto GetContext() const -> class Context* { return context; }

protected:
    class Context* context;
};