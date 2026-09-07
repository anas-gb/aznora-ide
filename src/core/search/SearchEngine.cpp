#include "core/search/SearchEngine.hpp"

#include <algorithm>
#include <fstream>
#include <system_error>

namespace aznora::core {

SearchEngine::SearchEngine()
{
    // TODO: Add search configuration and indexing support.
}

std::vector<SearchResult> SearchEngine::search(
    const std::filesystem::path& root,
    const std::string& query) const
{
    namespace fs = std::filesystem;

    std::vector<SearchResult> results;

    if (query.empty())
        return results;

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

        if (entry.is_regular_file(typeError) && !typeError)
        {
            std::ifstream file(entry.path());

            if (file)
            {
                std::string text;
                std::size_t lineNumber = 1;

                while (std::getline(file, text))
                {
                    if (text.find(query) != std::string::npos)
                    {
                        results.push_back({entry.path(), lineNumber, text});
                    }

                    ++lineNumber;
                }
            }
        }

        iterator.increment(error);
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const SearchResult& left, const SearchResult& right) {
            if (left.file != right.file)
                return left.file < right.file;

            return left.line < right.line;
        });

    return results;
}

} // namespace aznora::core