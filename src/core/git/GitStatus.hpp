#pragma once

#include <string>

namespace aznora::core {

struct GitStatus
{
    bool success;
    bool clean;
    std::string output;
};

} // namespace aznora::core