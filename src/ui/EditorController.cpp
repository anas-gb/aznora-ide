#include "ui/EditorController.hpp"
#include "ui/SyntaxHighlighter.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextEdit>
#include <QWidget>

#include <system_error>

namespace aznora::ui {

EditorController::EditorController(
    QTabWidget* tabWidget,
    aznora::core::EditorManager& editorManager,
    QObject* parent)
    : QObject(parent)
    , m_tabWidget(tabWidget)
    , m_editorManager(editorManager)
{
    if (m_tabWidget)
    {
        m_tabWidget->setTabsClosable(true);
        m_tabWidget->setMovable(true);

        connect(
            m_tabWidget,
            &QTabWidget::currentChanged,
            this,
            &EditorController::handleCurrentTabChanged);

        connect(
            m_tabWidget,
            &QTabWidget::tabCloseRequested,
            this,
            &EditorController::handleTabCloseRequested);
    }
}

EditorController::~EditorController() = default;

bool EditorController::openFile(const std::filesystem::path& filePath)
{
    std::error_code ec;
    if (std::filesystem::is_directory(filePath, ec) || ec)
        return false;

    // Check if already open
    const int existingIndex = findTabIndex(filePath);
    if (existingIndex >= 0)
    {
        m_tabWidget->setCurrentIndex(existingIndex);
        if (m_tabs[existingIndex].editor)
            m_tabs[existingIndex].editor->setFocus();
        return true;
    }

    const std::optional<aznora::core::TextDocument> document =
        m_editorManager.openFile(filePath);

    if (!document)
        return false;

    auto* editor = new QTextEdit(m_tabWidget);
    editor->setObjectName(QStringLiteral("EditorArea"));

    QFont editorFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    editorFont.setPointSize(12);
    editor->setFont(editorFont);
    editor->setPlainText(QString::fromStdString(document->content));

    const aznora::core::Language language = m_languageDetector.detect(filePath);
    auto highlighter = std::make_unique<SyntaxHighlighter>(editor->document(), language);

    connect(editor, &QTextEdit::textChanged, this, &EditorController::handleTextChanged);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &EditorController::handleCursorPositionChanged);

    const QString fileName = QString::fromStdString(filePath.filename().string());
    const int tabIndex = m_tabWidget->addTab(editor, fileName);

    m_tabs.push_back({
        filePath,
        editor,
        std::move(highlighter),
        false,
        language
    });

    m_tabWidget->setCurrentIndex(tabIndex);
    editor->setFocus();

    updateWindowTitle();
    emit fileOpened(filePath);
    emit currentFileChanged(filePath);
    emit currentLanguageChanged(language);
    handleCursorPositionChanged();

    return true;
}

bool EditorController::saveCurrentFile()
{
    if (!m_tabWidget)
        return false;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0 || currentIndex >= static_cast<int>(m_tabs.size()))
        return false;

    DocumentTab& tab = m_tabs[currentIndex];
    if (!tab.editor)
        return false;

    const std::string content = tab.editor->toPlainText().toStdString();
    const bool saved = m_editorManager.saveFile({tab.filePath, content, false});

    if (saved)
    {
        tab.isModified = false;
        updateTabTitle(currentIndex);
        updateWindowTitle();
        emit fileSaved(tab.filePath);
        return true;
    }

    return false;
}

bool EditorController::closeTab(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tabs.size()))
        return false;

    m_tabs.erase(m_tabs.begin() + index);
    m_tabWidget->removeTab(index);

    if (m_tabs.empty())
    {
        if (QWidget* win = m_tabWidget->window())
            win->setWindowTitle(QStringLiteral("Aznora IDE"));

        emit currentLanguageChanged(aznora::core::Language::Unknown);
        emit cursorPositionChanged(0, 0);
    }
    else
    {
        updateWindowTitle();
    }

    return true;
}

void EditorController::jumpToLine(int line)
{
    QTextEdit* editor = currentEditor();
    if (!editor || line <= 0)
        return;

    const QTextBlock block = editor->document()->findBlockByLineNumber(line - 1);
    if (block.isValid())
    {
        QTextCursor cursor(block);
        editor->setTextCursor(cursor);
        editor->ensureCursorVisible();
        editor->setFocus();
    }
}

std::optional<std::filesystem::path> EditorController::currentFilePath() const
{
    if (!m_tabWidget)
        return std::nullopt;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < static_cast<int>(m_tabs.size()))
        return m_tabs[currentIndex].filePath;

    return std::nullopt;
}

bool EditorController::hasOpenFile() const
{
    return currentFilePath().has_value();
}

aznora::core::Language EditorController::currentLanguage() const
{
    if (!m_tabWidget)
        return aznora::core::Language::Unknown;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < static_cast<int>(m_tabs.size()))
        return m_tabs[currentIndex].language;

    return aznora::core::Language::Unknown;
}

QTextEdit* EditorController::currentEditor() const
{
    if (!m_tabWidget)
        return nullptr;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < static_cast<int>(m_tabs.size()))
        return m_tabs[currentIndex].editor;

    return nullptr;
}

void EditorController::handleCurrentTabChanged(int index)
{
    if (index >= 0 && index < static_cast<int>(m_tabs.size()))
    {
        const DocumentTab& tab = m_tabs[index];
        updateWindowTitle();
        emit currentFileChanged(tab.filePath);
        emit currentLanguageChanged(tab.language);
        handleCursorPositionChanged();
    }
    else
    {
        if (QWidget* win = m_tabWidget ? m_tabWidget->window() : nullptr)
            win->setWindowTitle(QStringLiteral("Aznora IDE"));
        emit currentLanguageChanged(aznora::core::Language::Unknown);
        emit cursorPositionChanged(0, 0);
    }
}

void EditorController::handleTabCloseRequested(int index)
{
    closeTab(index);
}

void EditorController::handleTextChanged()
{
    if (!m_tabWidget)
        return;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0 || currentIndex >= static_cast<int>(m_tabs.size()))
        return;

    DocumentTab& tab = m_tabs[currentIndex];
    if (!tab.isModified)
    {
        tab.isModified = true;
        updateTabTitle(currentIndex);
        updateWindowTitle();
    }
}

void EditorController::handleCursorPositionChanged()
{
    QTextEdit* editor = currentEditor();
    if (!editor)
    {
        emit cursorPositionChanged(0, 0);
        return;
    }

    const QTextCursor cursor = editor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int col = cursor.positionInBlock() + 1;
    emit cursorPositionChanged(line, col);
}

void EditorController::updateWindowTitle()
{
    if (!m_tabWidget)
        return;

    QWidget* win = m_tabWidget->window();
    if (!win)
        return;

    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0 && currentIndex < static_cast<int>(m_tabs.size()))
    {
        const DocumentTab& tab = m_tabs[currentIndex];
        const QString fileName = QString::fromStdString(tab.filePath.filename().string());
        const QString modPrefix = tab.isModified ? QStringLiteral("*") : QString();
        win->setWindowTitle(QStringLiteral("Aznora IDE - ") + fileName + modPrefix);
    }
    else
    {
        win->setWindowTitle(QStringLiteral("Aznora IDE"));
    }
}

void EditorController::updateTabTitle(int index)
{
    if (!m_tabWidget || index < 0 || index >= static_cast<int>(m_tabs.size()))
        return;

    const DocumentTab& tab = m_tabs[index];
    const QString fileName = QString::fromStdString(tab.filePath.filename().string());
    const QString dirtyMark = tab.isModified ? QStringLiteral(" *") : QString();
    m_tabWidget->setTabText(index, fileName + dirtyMark);
}

int EditorController::findTabIndex(const std::filesystem::path& filePath) const
{
    for (std::size_t i = 0; i < m_tabs.size(); ++i)
    {
        if (m_tabs[i].filePath == filePath)
            return static_cast<int>(i);
    }
    return -1;
}

} // namespace aznora::ui
