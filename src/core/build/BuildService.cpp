#include "core/build/BuildService.hpp"

#include "core/terminal/TerminalService.hpp"

#include <filesystem>
#include <string>
#include <system_error>

namespace {

std::string quoteForShell(const std::filesystem::path& path)
{
    std::string value = path.string();

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

aznora::core::BuildResult failureResult(const std::string& output)
{
    return {false, -1, output};
}

} // namespace

namespace aznora::core {

BuildService::BuildService()
{
    // TODO: Add build presets and toolchain configuration.
}

BuildResult BuildService::buildProject(
    const std::filesystem::path& projectPath) const
{
    namespace fs = std::filesystem;

    if (!fs::is_regular_file(projectPath / "CMakeLists.txt"))
        return failureResult("CMakeLists.txt was not found in the project.\n");

    const fs::path buildPath = projectPath / "build";
    TerminalService terminal;
    const CommandResult configure = terminal.execute(
        "cmake -S " + quoteForShell(projectPath) +
        " -B " + quoteForShell(buildPath));

    if (configure.exitCode != 0)
        return {false, configure.exitCode, configure.output};

    const CommandResult build = terminal.execute(
        "cmake --build " + quoteForShell(buildPath));

    return {
        build.exitCode == 0,
        build.exitCode,
        configure.output + build.output};
}

BuildResult BuildService::runProject(
    const std::filesystem::path& projectPath) const
{
    namespace fs = std::filesystem;

    const fs::path buildPath = projectPath / "build";
    const fs::path executable = [&]() {
        const fs::path nestedExecutable =
            buildPath / "apps" / "aznora-desktop" / "Aznora";

        if (fs::is_regular_file(nestedExecutable))
            return nestedExecutable;

        return buildPath / "Aznora";
    }();

    if (!fs::is_regular_file(executable))
        return failureResult("No runnable project executable was found in build/.\n");

    TerminalService terminal;
    const CommandResult result = terminal.execute(quoteForShell(executable));

    return {result.exitCode == 0, result.exitCode, result.output};
}

} // namespace aznora::core