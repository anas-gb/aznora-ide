#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "Workspace.hpp"

class WorkspaceManager
{
public:
    bool createWorkspace(
        const std::string& name,
        const std::filesystem::path& location);

    bool validateWorkspace(const std::filesystem::path& path) const;

    std::optional<Workspace> openWorkspace(
        const std::filesystem::path& path);

private:
    bool updateRecentWorkspaces(const std::filesystem::path& path) const;
};