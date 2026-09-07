#pragma once

#include <filesystem>
#include <optional>

#include "core/editor/TextDocument.hpp"

namespace aznora::core {

class EditorManager
{
public:
    EditorManager();

    std::optional<TextDocument> openFile(
        const std::filesystem::path& path) const;

    bool saveFile(const TextDocument& document) const;
};

} // namespace aznora::core