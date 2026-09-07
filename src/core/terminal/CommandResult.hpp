#pragma once

#include <string>

namespace aznora::core {

struct CommandResult
{
    int exitCode;
    std::string output;
};

} // namespace aznora::core