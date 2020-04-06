#include "Framework.h"
#include "GUID_Generator.h"
#include <sstream>
#include <iomanip>
#include <guiddef.h>

auto GUID_Generator::Generate() -> const uint
{
    std::hash<std::string> hasher;

    return static_cast<uint>(hasher(GenerateToString()));
}

auto GUID_Generator::GenerateToString() -> const std::string
{
    GUID guid;
    auto hr = CoCreateGuid(&guid);

    if (SUCCEEDED(hr))
    {
        std::stringstream sstream;        

        sstream
            << std::hex << std::uppercase
            << std::setw(8) << std::setfill('0') << guid.Data1 << "-"
            << std::setw(4) << std::setfill('0') << guid.Data2 << "-"
            << std::setw(4) << std::setfill('0') << guid.Data3 << "-"
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[0])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[1])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[2])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[3])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[4])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[5])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[6])
            << std::setw(2) << std::setfill('0') << static_cast<uint>(guid.Data4[7]);

        return sstream.str();
    }

    return NOT_ASSIGNED_STR;
}