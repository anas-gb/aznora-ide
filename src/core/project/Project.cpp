#include "core/project/Project.hpp"

#include <utility>

namespace aznora::core {

Project::Project(std::filesystem::path projectPath)
    : projectPath_(std::move(projectPath)) {
    // TODO: Load project metadata and configuration.
}

const std::filesystem::path& Project::path() const noexcept {
    return projectPath_;
}

} // namespace aznora::core