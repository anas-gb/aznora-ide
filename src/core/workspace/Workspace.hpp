#pragma once

#include <filesystem>

namespace aznora::core {

class Workspace {
public:
    explicit Workspace(std::filesystem::path rootPath);

    const std::filesystem::path& rootPath() const noexcept;

private:
    std::filesystem::path rootPath_;
};

} // namespace aznora::core