#pragma once

#include <filesystem>
#include <vector>

#include "core/filesystem/FileEntry.hpp"

namespace aznora::core {

class FileExplorer
{
public:
    FileExplorer();

    std::vector<FileEntry> listFiles(
        const std::filesystem::path& root) const;
};

} // namespace aznora::core