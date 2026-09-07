#include "core/workspace/Workspace.hpp"

#include <utility>

namespace aznora::core {

Workspace::Workspace(std::filesystem::path rootPath)
    : rootPath_(std::move(rootPath)) {
    // TODO: Validate and load workspace metadata.
}

const std::filesystem::path& Workspace::rootPath() const noexcept {
    return rootPath_;
}

} // namespace aznora::core