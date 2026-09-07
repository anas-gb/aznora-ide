#pragma once

#include <filesystem>
#include <string>

struct Workspace
{
    std::string name;
    std::string version;
    std::filesystem::path rootPath;
};