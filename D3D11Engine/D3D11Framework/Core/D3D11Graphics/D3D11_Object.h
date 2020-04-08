#pragma once
#include "D3D11Framework.h"

class D3D11_Object
{
public:
    D3D11_Object() { id = GUID_Generator::Generate(); }
    virtual ~D3D11_Object() = default;

    auto GetID() const -> const uint& { return id; }

private:
    uint id;
};