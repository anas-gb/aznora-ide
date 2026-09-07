#include "WorkspaceManager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>
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

bool WorkspaceManager::validateWorkspace(const std::filesystem::path& path) const
{
    namespace fs = std::filesystem;

    const fs::path metadataPath = path / ".aznora" / "workspace.json";

    return fs::is_directory(path) && fs::is_directory(path / ".aznora") &&
           fs::is_regular_file(metadataPath);
}

std::optional<Workspace> WorkspaceManager::openWorkspace(
    const std::filesystem::path& path)
{
    if (!validateWorkspace(path))
        return std::nullopt;

    const std::filesystem::path metadataPath = path / ".aznora" / "workspace.json";

    std::ifstream workspace(metadataPath);

    if (!workspace)
        return std::nullopt;

    const std::string content(
        (std::istreambuf_iterator<char>(workspace)),
        std::istreambuf_iterator<char>());
    const std::string name = readJsonValue(content, "name");
    const std::string version = readJsonValue(content, "version");

    if (name.empty() || version.empty())
        return std::nullopt;

    const Workspace currentWorkspace{name, version, path};

    std::cout << "Opening workspace...\n"
              << "Name: " << name << '\n'
              << "Version: " << version << std::endl;

    updateRecentWorkspaces(path);
    return currentWorkspace;
}

bool WorkspaceManager::updateRecentWorkspaces(
    const std::filesystem::path& path) const
{
    const char* home = std::getenv("HOME");

    if (home == nullptr || *home == '\0')
        return false;

    namespace fs = std::filesystem;
    const fs::path settingsDirectory = fs::path(home) / ".aznora";
    const fs::path recentPath = settingsDirectory / "recent.json";

    std::error_code error;
    fs::create_directories(settingsDirectory, error);

    if (error)
        return false;

    std::ofstream recent(recentPath);

    if (!recent)
        return false;

    recent << "[\n    \"" << path.string() << "\"\n]\n";
    return true;
}