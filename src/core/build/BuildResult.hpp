#pragma once

#include <string>

namespace aznora::core {

struct BuildResult
{
    bool success;
    int exitCode;
    std::string output;
};

} // namespace aznora::core