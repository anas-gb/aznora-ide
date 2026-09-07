#include "core/languages/LanguageDetector.hpp"

#include <algorithm>
#include <cctype>
#include <string>

namespace aznora::core {

LanguageDetector::LanguageDetector()
{
    // TODO: Add configurable language and extension associations.
}

Language LanguageDetector::detect(const std::filesystem::path& file) const
{
    std::string extension = file.extension().string();
    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](const unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });

    if (extension == ".cpp" || extension == ".cc" || extension == ".cxx" ||
        extension == ".h" || extension == ".hpp" || extension == ".hxx")
        return Language::Cpp;

    if (extension == ".py")
        return Language::Python;

    if (extension == ".js" || extension == ".mjs" || extension == ".cjs")
        return Language::JavaScript;

    return Language::Unknown;
}

} // namespace aznora::core