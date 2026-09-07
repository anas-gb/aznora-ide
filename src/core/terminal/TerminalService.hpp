#pragma once

#include <string>

#include "core/terminal/CommandResult.hpp"

namespace aznora::core {

class TerminalService
{
public:
    TerminalService();

    CommandResult execute(const std::string& command) const;
};

} // namespace aznora::core