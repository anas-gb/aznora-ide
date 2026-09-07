#include "core/filesystem/FileSystemService.hpp"

namespace aznora::core {

FileSystemService::FileSystemService() {
    // TODO: Configure filesystem access and error handling.
}

bool FileSystemService::exists(const std::filesystem::path& path) const {
    // TODO: Add project-specific filesystem policy.
    return std::filesystem::exists(path);
}

} // namespace aznora::core