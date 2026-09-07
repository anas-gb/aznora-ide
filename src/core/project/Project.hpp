#pragma once

#include <filesystem>

namespace aznora::core {

class Project {
public:
    explicit Project(std::filesystem::path projectPath);

    const std::filesystem::path& path() const noexcept;

private:
    std::filesystem::path projectPath_;
};

} // namespace aznora::core