#pragma once

#include <QMainWindow>
#include <filesystem>
#include <memory>

#include "core/build/BuildService.hpp"
#include "core/editor/EditorManager.hpp"
#include "core/git/GitService.hpp"
#include "core/languages/Language.hpp"
#include "core/search/SearchEngine.hpp"
#include "core/terminal/TerminalService.hpp"

class QAction;
class QDockWidget;
class QLabel;
class QMenuBar;
class QTabWidget;
class QTreeWidget;

namespace aznora::ui {

class BuildController;
class EditorController;
class GitController;
class SearchController;
class TerminalController;
class WorkspaceTreeController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void loadWorkspace(const std::filesystem::path& workspacePath);

    [[nodiscard]] EditorController* editorController() const;
    [[nodiscard]] TerminalController* terminalController() const;
    [[nodiscard]] BuildController* buildController() const;
    [[nodiscard]] SearchController* searchController() const;
    [[nodiscard]] GitController* gitController() const;

public slots:
    void setDarkTheme();
    void setLightTheme();

private slots:
    void handleCursorPositionChanged(int line, int col);
    void handleLanguageChanged(aznora::core::Language language);
    void handleGitStatusUpdated(bool clean, const QString& statusText);

private:
    void setupUi();
    void setupMenuBar();
    void setupStatusBar();
    void setupExplorerDock();
    void setupSearchDock();
    void setupGitDock();
    void setupEditorArea();
    void setupTerminalDock();
    void setupControllers();
    void applyTheme(bool dark);

    // Backend Services
    aznora::core::EditorManager m_editorManager;
    aznora::core::TerminalService m_terminalService;
    aznora::core::BuildService m_buildService;
    aznora::core::SearchEngine m_searchEngine;
    aznora::core::GitService m_gitService;

    // UI Widgets
    QDockWidget* m_explorerDock{nullptr};
    QDockWidget* m_searchDock{nullptr};
    QDockWidget* m_gitDock{nullptr};
    QDockWidget* m_terminalDock{nullptr};

    QTreeWidget* m_explorerTree{nullptr};
    QTabWidget* m_editorTabs{nullptr};

    QLabel* m_cursorPosLabel{nullptr};
    QLabel* m_languageLabel{nullptr};
    QLabel* m_gitStatusLabel{nullptr};

    QAction* m_darkThemeAction{nullptr};
    QAction* m_lightThemeAction{nullptr};

    // Controllers
    std::unique_ptr<WorkspaceTreeController> m_workspaceTreeController;
    std::unique_ptr<EditorController> m_editorController;
    std::unique_ptr<TerminalController> m_terminalController;
    std::unique_ptr<BuildController> m_buildController;
    std::unique_ptr<SearchController> m_searchController;
    std::unique_ptr<GitController> m_gitController;
};

} // namespace aznora::ui
