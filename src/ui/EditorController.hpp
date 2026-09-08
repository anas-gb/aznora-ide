#pragma once

#include <QObject>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

#include "core/editor/EditorManager.hpp"
#include "core/languages/Language.hpp"
#include "core/languages/LanguageDetector.hpp"

class QTabWidget;
class QTextEdit;

namespace aznora::ui {

class SyntaxHighlighter;

class EditorController : public QObject
{
    Q_OBJECT

public:
    struct DocumentTab
    {
        std::filesystem::path filePath;
        QTextEdit* editor{nullptr};
        std::unique_ptr<SyntaxHighlighter> highlighter;
        bool isModified{false};
        aznora::core::Language language{aznora::core::Language::Unknown};
    };

    EditorController(
        QTabWidget* tabWidget,
        aznora::core::EditorManager& editorManager,
        QObject* parent = nullptr);

    ~EditorController() override;

    [[nodiscard]] std::optional<std::filesystem::path> currentFilePath() const;
    [[nodiscard]] bool hasOpenFile() const;
    [[nodiscard]] aznora::core::Language currentLanguage() const;
    [[nodiscard]] QTextEdit* currentEditor() const;

    void jumpToLine(int line);

public slots:
    bool openFile(const std::filesystem::path& filePath);
    bool saveCurrentFile();
    bool closeTab(int index);

signals:
    void fileOpened(const std::filesystem::path& filePath);
    void fileSaved(const std::filesystem::path& filePath);
    void currentFileChanged(const std::filesystem::path& filePath);
    void cursorPositionChanged(int line, int col);
    void currentLanguageChanged(aznora::core::Language language);

private slots:
    void handleCurrentTabChanged(int index);
    void handleTabCloseRequested(int index);
    void handleTextChanged();
    void handleCursorPositionChanged();

private:
    void updateWindowTitle();
    void updateTabTitle(int index);
    int findTabIndex(const std::filesystem::path& filePath) const;

    QTabWidget* m_tabWidget{nullptr};
    aznora::core::EditorManager& m_editorManager;
    aznora::core::LanguageDetector m_languageDetector;
    std::vector<DocumentTab> m_tabs;
};

} // namespace aznora::ui
