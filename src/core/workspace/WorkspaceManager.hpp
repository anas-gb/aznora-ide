#pragma once

#include <filesystem>
#include <string>

class WorkspaceManager
{
public:
    bool createWorkspace(
        const std::string& name,
        const std::filesystem::path& location);
};