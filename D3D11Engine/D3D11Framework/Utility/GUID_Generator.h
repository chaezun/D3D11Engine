#pragma once

//D3D11_Object���� ����ID�� �����ϴ� �κ�
class GUID_Generator final
{
public:
    static auto Generate() -> const uint;
    static auto GenerateToString() -> const std::string;
};