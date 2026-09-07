#include "core/editor/EditorManager.hpp"

#include <fstream>
#include <iterator>

namespace aznora::core {

EditorManager::EditorManager()
{
    // TODO: Track open documents and editor state.
}

std::optional<TextDocument> EditorManager::openFile(
    const std::filesystem::path& path) const
{
    std::ifstream file(path);

    if (!file)
        return std::nullopt;

    const std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    return TextDocument{path, content, false};
}

bool EditorManager::saveFile(const TextDocument& document) const
{
    std::ofstream file(document.path, std::ios::trunc);

    if (!file)
        return false;

    file << document.content;
    return file.good();
}

} // namespace aznora::core