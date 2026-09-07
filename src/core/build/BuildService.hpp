#pragma once

#include <filesystem>

#include "core/build/BuildResult.hpp"

namespace aznora::core {

class BuildService
{
public:
    BuildService();

    BuildResult buildProject(
        const std::filesystem::path& projectPath) const;

    BuildResult runProject(
        const std::filesystem::path& projectPath) const;
};

} // namespace aznora::core