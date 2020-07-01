#pragma once

//추상클래스 - 일반 클래스에 순수가상 함수가 하나 이상 포함된 것 -> 추상클래스는 따로 인스턴싱할 수 없음
//C++에서는 인터페이스가 따로 없기 떄문에 추상클래스의 성질을 이용하여
//공통된 부분을 순수 가상함수로 만들어 상속받는 클래스에서 구현하도록 함.
//이 프로젝트에서는 I라는 접두사가 붙으면 인터페이스의 역할을 함.
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