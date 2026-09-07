#pragma once

#include <filesystem>
#include <string>

#include "core/git/GitStatus.hpp"

namespace aznora::core {

class GitService
{
public:
    GitService();

    bool initRepository(const std::filesystem::path& path) const;

    bool cloneRepository(
        const std::string& url,
        const std::filesystem::path& destination) const;

    GitStatus status(const std::filesystem::path& path) const;

    bool commit(
        const std::filesystem::path& path,
        const std::string& message) const;

    bool push(const std::filesystem::path& path) const;

    bool pull(const std::filesystem::path& path) const;
};

} // namespace aznora::core