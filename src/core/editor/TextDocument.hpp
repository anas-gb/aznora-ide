#pragma once

#include <filesystem>
#include <string>

namespace aznora::core {

struct TextDocument
{
    std::filesystem::path path;
    std::string content;
    bool modified;
};

} // namespace aznora::core