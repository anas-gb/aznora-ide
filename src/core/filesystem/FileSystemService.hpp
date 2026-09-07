#pragma once

#include <filesystem>

namespace aznora::core {

class FileSystemService {
public:
    FileSystemService();

    bool exists(const std::filesystem::path& path) const;
};

} // namespace aznora::core