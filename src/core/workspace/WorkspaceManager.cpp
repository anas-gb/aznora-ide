#include "WorkspaceManager.hpp"

#include <filesystem>
#include <fstream>

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
    "version": "0.0.1"
})";

    return true;
}