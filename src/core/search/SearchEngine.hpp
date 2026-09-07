#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "core/search/SearchResult.hpp"

namespace aznora::core {

class SearchEngine
{
public:
    SearchEngine();

    std::vector<SearchResult> search(
        const std::filesystem::path& root,
        const std::string& query) const;
};

} // namespace aznora::core