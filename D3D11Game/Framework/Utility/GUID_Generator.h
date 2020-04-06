#pragma once

class GUID_Generator final
{
public:
    static auto Generate() -> const uint;
    static auto GenerateToString() -> const std::string;
};