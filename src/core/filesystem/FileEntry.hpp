#pragma once

#include <filesystem>
#include <string>

namespace aznora::core {

struct FileEntry
{
    std::string name;
    std::filesystem::path path;
    bool isDirectory;
};

} // namespace aznora::core