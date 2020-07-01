#pragma once

//D3D11_Object에서 고유ID를 생성하는 부분
class GUID_Generator final
{
public:
    static auto Generate() -> const uint;
    static auto GenerateToString() -> const std::string;
};