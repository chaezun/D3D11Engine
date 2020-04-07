#pragma once
#include "D3D11Framework.h"

class ILogger
{
public:
    virtual ~ILogger() = default;
    virtual void Log(const std::string& log, const uint& type) = 0;
};