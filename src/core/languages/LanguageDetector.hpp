#pragma once

#include <filesystem>

#include "core/languages/Language.hpp"

namespace aznora::core {

class LanguageDetector
{
public:
    LanguageDetector();

    Language detect(const std::filesystem::path& file) const;
};

} // namespace aznora::core