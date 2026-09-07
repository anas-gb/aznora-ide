#include "core/terminal/TerminalService.hpp"

#include <array>
#include <cstdio>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace aznora::core {

TerminalService::TerminalService()
{
    // TODO: Add working-directory and environment configuration.
}

CommandResult TerminalService::execute(const std::string& command) const
{
    const std::string commandWithErrors = command + " 2>&1";

#ifdef _WIN32
    FILE* pipe = _popen(commandWithErrors.c_str(), "r");
#else
    FILE* pipe = popen(commandWithErrors.c_str(), "r");
#endif

    if (pipe == nullptr)
        return {-1, {}};

    std::array<char, 4096> buffer{};
    std::string output;

    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
        output += buffer.data();

#ifdef _WIN32
    const int exitCode = _pclose(pipe);
#else
    const int status = pclose(pipe);
    const int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif

    return {exitCode, output};
}

} // namespace aznora::core