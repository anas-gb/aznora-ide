#pragma once

#include <filesystem>
#include <string>

namespace aznora::core {

struct SearchResult
{
    std::filesystem::path file;
    std::size_t line;
    std::string text;
};

} // namespace aznora::core