#include "core/git/GitService.hpp"

#include "core/terminal/TerminalService.hpp"

#include <string>

namespace {

std::string quoteForShell(const std::string& value)
{
#ifdef _WIN32
    return "\"" + value + "\"";
#else
    std::string quoted = "'";

    for (const char character : value)
    {
        if (character == '\'')
            quoted += "'\\''";
        else
            quoted += character;
    }

    return quoted + "'";
#endif
}

std::string repositoryCommand(
    const std::string& command,
    const std::filesystem::path& path)
{
    return "git -C " + quoteForShell(path.string()) + " " + command;
}

} // namespace

namespace aznora::core {

GitService::GitService()
{
    // TODO: Add Git executable discovery and configuration.
}

bool GitService::initRepository(const std::filesystem::path& path) const
{
    TerminalService terminal;
    return terminal.execute(repositoryCommand("init", path)).exitCode == 0;
}

bool GitService::cloneRepository(
    const std::string& url,
    const std::filesystem::path& destination) const
{
    TerminalService terminal;
    const std::string command =
        "git clone " + quoteForShell(url) + " " +
        quoteForShell(destination.string());

    return terminal.execute(command).exitCode == 0;
}

GitStatus GitService::status(const std::filesystem::path& path) const
{
    TerminalService terminal;
    const CommandResult result = terminal.execute(
        repositoryCommand("status --porcelain", path));

    return {
        result.exitCode == 0,
        result.exitCode == 0 && result.output.empty(),
        result.output};
}

bool GitService::commit(
    const std::filesystem::path& path,
    const std::string& message) const
{
    TerminalService terminal;
    const std::string command =
        repositoryCommand("add -A && ", path) +
        repositoryCommand("commit -m " + quoteForShell(message), path);

    return terminal.execute(command).exitCode == 0;
}

bool GitService::push(const std::filesystem::path& path) const
{
    TerminalService terminal;
    return terminal.execute(repositoryCommand("push", path)).exitCode == 0;
}

bool GitService::pull(const std::filesystem::path& path) const
{
    TerminalService terminal;
    return terminal.execute(repositoryCommand("pull", path)).exitCode == 0;
}

} // namespace aznora::core