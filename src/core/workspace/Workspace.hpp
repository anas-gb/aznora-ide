#pragma once
#include <filesystem>
#include <string>
struct workspace
{
    std::string name;
    std::filesystem::path path;
};