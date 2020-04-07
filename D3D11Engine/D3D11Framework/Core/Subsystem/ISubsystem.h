#pragma once

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