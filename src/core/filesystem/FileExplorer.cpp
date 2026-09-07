#include "core/filesystem/FileExplorer.hpp"

#include <algorithm>
#include <system_error>

namespace aznora::core {

FileExplorer::FileExplorer()
{
    // TODO: Configure filtering and filesystem access policies.
}

std::vector<FileEntry> FileExplorer::listFiles(
    const std::filesystem::path& root) const
{
    namespace fs = std::filesystem;

    std::vector<FileEntry> files;
    std::error_code error;
    fs::recursive_directory_iterator iterator(
        root,
        fs::directory_options::skip_permission_denied,
        error);
    const fs::recursive_directory_iterator end;

    while (iterator != end && !error)
    {
        const fs::directory_entry& entry = *iterator;
        std::error_code typeError;
        const bool isDirectory = entry.is_directory(typeError);

        if (!typeError)
        {
            files.push_back({
                entry.path().filename().string(),
                entry.path(),
                isDirectory});
        }

        iterator.increment(error);
    }

    std::sort(
        files.begin(),
        files.end(),
        [](const FileEntry& left, const FileEntry& right) {
            return left.path < right.path;
        });

    return files;
}

} // namespace aznora::core