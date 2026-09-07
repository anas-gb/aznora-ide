#include "WorkspaceManager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {

std::string readJsonValue(const std::string& content, const std::string& key)
{
    const std::string keyToken = "\"" + key + "\"";
    const std::size_t keyPosition = content.find(keyToken);

    if (keyPosition == std::string::npos)
        return {};

    const std::size_t separatorPosition = content.find(':', keyPosition + keyToken.size());
    const std::size_t valueStart = content.find('"', separatorPosition);

    if (separatorPosition == std::string::npos || valueStart == std::string::npos)
        return {};

    const std::size_t valueEnd = content.find('"', valueStart + 1);

    if (valueEnd == std::string::npos)
        return {};

    return content.substr(valueStart + 1, valueEnd - valueStart - 1);
}

} // namespace

bool WorkspaceManager::createWorkspace(
    const std::string& name,
    const std::filesystem::path& location)
{
    namespace fs = std::filesystem;

    fs::path projectPath = location / name;

    if (fs::exists(projectPath))
        return false;

    fs::create_directories(projectPath);
    fs::create_directories(projectPath / ".aznora");
    fs::create_directories(projectPath / "src");

    std::ofstream readme(projectPath / "README.md");
    readme << "# " << name << "\n";
    readme << "Created with Aznora IDE\n";

    std::ofstream workspace(projectPath / ".aznora" / "workspace.json");
    workspace <<
R"({
    "name": ")" << name << R"(",
    "version": "0.1.0",
    "creator": "Aznora IDE"
})";

    return true;
}

bool WorkspaceManager::openWorkspace(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    const fs::path metadataPath = path / ".aznora" / "workspace.json";

    if (!fs::is_directory(path) || !fs::is_directory(path / ".aznora") ||
        !fs::is_regular_file(metadataPath))
        return false;

    std::ifstream workspace(metadataPath);

    if (!workspace)
        return false;

    const std::string content(
        (std::istreambuf_iterator<char>(workspace)),
        std::istreambuf_iterator<char>());
    const std::string name = readJsonValue(content, "name");
    const std::string version = readJsonValue(content, "version");

    if (name.empty() || version.empty())
        return false;

    std::cout << "Opening workspace...\n"
              << "Name: " << name << '\n'
              << "Version: " << version << std::endl;

    return true;
}