#include "ui/MainWindow.hpp"
#include "ui/BuildController.hpp"
#include "ui/EditorController.hpp"
#include "ui/GitController.hpp"
#include "ui/SearchController.hpp"
#include "ui/TerminalController.hpp"
#include "ui/WorkspaceTreeController.hpp"

#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QFontDatabase>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTreeWidget>

namespace aznora::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Aznora IDE"));
    resize(1400, 900);

    setupUi();
    setupControllers();
    applyTheme(true);

    loadWorkspace(std::filesystem::current_path());
}

MainWindow::~MainWindow() = default;

void MainWindow::loadWorkspace(const std::filesystem::path& workspacePath)
{
    if (m_workspaceTreeController)
        m_workspaceTreeController->loadWorkspace(workspacePath);

    if (m_buildController)
        m_buildController->setWorkspacePath(workspacePath);

    if (m_searchController)
        m_searchController->setWorkspacePath(workspacePath);

    if (m_gitController)
        m_gitController->setWorkspacePath(workspacePath);
}

EditorController* MainWindow::editorController() const
{
    return m_editorController.get();
}

TerminalController* MainWindow::terminalController() const
{
    return m_terminalController.get();
}

BuildController* MainWindow::buildController() const
{
    return m_buildController.get();
}

SearchController* MainWindow::searchController() const
{
    return m_searchController.get();
}

GitController* MainWindow::gitController() const
{
    return m_gitController.get();
}

void MainWindow::setupUi()
{
    setupMenuBar();
    setupStatusBar();
    setupExplorerDock();
    setupSearchDock();
    setupGitDock();
    setupEditorArea();
    setupTerminalDock();

    tabifyDockWidget(m_explorerDock, m_searchDock);
    tabifyDockWidget(m_searchDock, m_gitDock);
    m_explorerDock->raise();

    // View Menu toggle actions
    if (QMenuBar* menu = menuBar()) {
        for (QAction* action : menu->actions()) {
            if (action->text() == tr("&View") && action->menu()) {
                QMenu* viewMenu = action->menu();
                viewMenu->addAction(m_explorerDock->toggleViewAction());
                viewMenu->addAction(m_searchDock->toggleViewAction());
                viewMenu->addAction(m_gitDock->toggleViewAction());
                viewMenu->addAction(m_terminalDock->toggleViewAction());
                break;
            }
        }
    }

    resizeDocks({m_explorerDock}, {280}, Qt::Horizontal);
    resizeDocks({m_terminalDock}, {220}, Qt::Vertical);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menu = menuBar();

    // File Menu
    QMenu* fileMenu = menu->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New File"), QStringLiteral("Ctrl+N"));
    
    QAction* openFileAction = fileMenu->addAction(tr("&Open File..."), QStringLiteral("Ctrl+O"));
    connect(openFileAction, &QAction::triggered, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(this, tr("Open File"));
        if (!path.isEmpty() && m_editorController)
            m_editorController->openFile(std::filesystem::path(path.toStdString()));
    });

    QAction* openFolderAction = fileMenu->addAction(tr("Open &Folder..."));
    connect(openFolderAction, &QAction::triggered, this, [this]() {
        const QString dir = QFileDialog::getExistingDirectory(this, tr("Open Workspace Folder"));
        if (!dir.isEmpty())
            loadWorkspace(std::filesystem::path(dir.toStdString()));
    });

    fileMenu->addSeparator();

    QAction* saveAction = fileMenu->addAction(tr("&Save"), QStringLiteral("Ctrl+S"));
    connect(saveAction, &QAction::triggered, this, [this]() {
        if (m_editorController)
            m_editorController->saveCurrentFile();
    });

    QAction* closeTabAction = fileMenu->addAction(tr("&Close Tab"), QStringLiteral("Ctrl+W"));
    connect(closeTabAction, &QAction::triggered, this, [this]() {
        if (m_editorController && m_editorTabs)
            m_editorController->closeTab(m_editorTabs->currentIndex());
    });

    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(tr("E&xit"), QStringLiteral("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Edit Menu
    QMenu* editMenu = menu->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), QStringLiteral("Ctrl+Z"));
    editMenu->addAction(tr("&Redo"), QStringLiteral("Ctrl+Y"));
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), QStringLiteral("Ctrl+X"));
    editMenu->addAction(tr("&Copy"), QStringLiteral("Ctrl+C"));
    editMenu->addAction(tr("&Paste"), QStringLiteral("Ctrl+V"));
    editMenu->addSeparator();
    QAction* findAction = editMenu->addAction(tr("&Find in Files"), QStringLiteral("Ctrl+Shift+F"));
    connect(findAction, &QAction::triggered, this, [this]() {
        if (m_searchDock) {
            m_searchDock->show();
            m_searchDock->raise();
        }
    });

    // View Menu
    QMenu* viewMenu = menu->addMenu(tr("&View"));
    QMenu* themeMenu = viewMenu->addMenu(tr("&Theme"));
    auto* themeGroup = new QActionGroup(this);

    m_darkThemeAction = themeMenu->addAction(tr("&Dark Theme"));
    m_darkThemeAction->setCheckable(true);
    m_darkThemeAction->setChecked(true);
    themeGroup->addAction(m_darkThemeAction);
    connect(m_darkThemeAction, &QAction::triggered, this, &MainWindow::setDarkTheme);

    m_lightThemeAction = themeMenu->addAction(tr("&Light Theme"));
    m_lightThemeAction->setCheckable(true);
    themeGroup->addAction(m_lightThemeAction);
    connect(m_lightThemeAction, &QAction::triggered, this, &MainWindow::setLightTheme);

    viewMenu->addSeparator();

    // Build Menu
    QMenu* buildMenu = menu->addMenu(tr("&Build"));
    QAction* buildAction = buildMenu->addAction(tr("&Build Project"), QStringLiteral("Ctrl+B"));
    connect(buildAction, &QAction::triggered, this, [this]() {
        if (m_terminalDock)
            m_terminalDock->show();
        if (m_buildController)
            m_buildController->buildProject();
    });

    QAction* runAction = buildMenu->addAction(tr("&Run Project"), QStringLiteral("Ctrl+R"));
    connect(runAction, &QAction::triggered, this, [this]() {
        if (m_terminalDock)
            m_terminalDock->show();
        if (m_buildController)
            m_buildController->runProject();
    });

    // Git Menu
    QMenu* gitMenu = menu->addMenu(tr("&Git"));
    QAction* gitRefresh = gitMenu->addAction(tr("&Status"));
    connect(gitRefresh, &QAction::triggered, this, [this]() {
        if (m_gitDock) {
            m_gitDock->show();
            m_gitDock->raise();
        }
        if (m_gitController)
            m_gitController->refreshStatus();
    });

    QAction* gitCommit = gitMenu->addAction(tr("&Commit..."));
    connect(gitCommit, &QAction::triggered, this, [this]() {
        if (m_gitDock) {
            m_gitDock->show();
            m_gitDock->raise();
        }
        if (m_gitController)
            m_gitController->commit();
    });

    QAction* gitPush = gitMenu->addAction(tr("&Push"));
    connect(gitPush, &QAction::triggered, this, [this]() {
        if (m_gitController)
            m_gitController->push();
    });

    QAction* gitPull = gitMenu->addAction(tr("P&ull"));
    connect(gitPull, &QAction::triggered, this, [this]() {
        if (m_gitController)
            m_gitController->pull();
    });

    // Help Menu
    QMenu* helpMenu = menu->addMenu(tr("&Help"));
    QAction* aboutAction = helpMenu->addAction(tr("&About Aznora IDE"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(
            this,
            tr("About Aznora IDE"),
            tr("<h2>Aznora IDE</h2>"
               "<p>Version 0.3.0 (Alpha)</p>"
               "<p>A native C++20 desktop IDE built with Qt6.</p>"
               "<p>Features: Multi-tab editor, Syntax Highlighting, Integrated Terminal, Build/Run pipeline, Search in Files, and Git Panel.</p>"));
    });
}

void MainWindow::setupStatusBar()
{
    QStatusBar* status = statusBar();
    status->showMessage(tr("Ready"));

    m_cursorPosLabel = new QLabel(QStringLiteral("Ln 1, Col 1"), this);
    m_cursorPosLabel->setMinimumWidth(100);
    m_cursorPosLabel->setAlignment(Qt::AlignCenter);
    status->addPermanentWidget(m_cursorPosLabel);

    m_languageLabel = new QLabel(QStringLiteral("Plain Text"), this);
    m_languageLabel->setMinimumWidth(90);
    m_languageLabel->setAlignment(Qt::AlignCenter);
    status->addPermanentWidget(m_languageLabel);

    m_gitStatusLabel = new QLabel(QStringLiteral("Git: Clean"), this);
    m_gitStatusLabel->setMinimumWidth(100);
    m_gitStatusLabel->setAlignment(Qt::AlignCenter);
    status->addPermanentWidget(m_gitStatusLabel);
}

void MainWindow::setupExplorerDock()
{
    m_explorerDock = new QDockWidget(tr("Explorer"), this);
    m_explorerDock->setObjectName(QStringLiteral("ExplorerDock"));
    m_explorerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_explorerTree = new QTreeWidget(m_explorerDock);
    m_explorerTree->setObjectName(QStringLiteral("ExplorerTree"));
    m_explorerTree->setHeaderHidden(true);

    m_explorerDock->setWidget(m_explorerTree);
    addDockWidget(Qt::LeftDockWidgetArea, m_explorerDock);
}

void MainWindow::setupSearchDock()
{
    m_searchDock = new QDockWidget(tr("Find in Files"), this);
    m_searchDock->setObjectName(QStringLiteral("SearchDock"));
    m_searchDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_searchDock);
}

void MainWindow::setupGitDock()
{
    m_gitDock = new QDockWidget(tr("Git"), this);
    m_gitDock->setObjectName(QStringLiteral("GitDock"));
    m_gitDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_gitDock);
}

void MainWindow::setupEditorArea()
{
    m_editorTabs = new QTabWidget(this);
    m_editorTabs->setObjectName(QStringLiteral("EditorTabs"));
    m_editorTabs->setDocumentMode(true);
    setCentralWidget(m_editorTabs);
}

void MainWindow::setupTerminalDock()
{
    m_terminalDock = new QDockWidget(tr("Terminal"), this);
    m_terminalDock->setObjectName(QStringLiteral("TerminalDock"));
    m_terminalDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_terminalDock);
}

void MainWindow::setupControllers()
{
    m_workspaceTreeController =
        std::make_unique<WorkspaceTreeController>(m_explorerTree, this);

    m_editorController =
        std::make_unique<EditorController>(m_editorTabs, m_editorManager, this);

    m_terminalController =
        std::make_unique<TerminalController>(m_terminalDock, m_terminalService, this);

    m_buildController =
        std::make_unique<BuildController>(m_buildService, *m_terminalController, this);

    m_searchController =
        std::make_unique<SearchController>(m_searchDock, m_searchEngine, this);

    m_gitController =
        std::make_unique<GitController>(m_gitDock, m_gitService, this);

    // Wire tree activation to editor open
    connect(
        m_workspaceTreeController.get(),
        &WorkspaceTreeController::fileDoubleClicked,
        m_editorController.get(),
        &EditorController::openFile);

    // Wire search result jump to editor
    connect(
        m_searchController.get(),
        &SearchController::resultSelected,
        this,
        [this](const std::filesystem::path& path, int line) {
            if (m_editorController) {
                m_editorController->openFile(path);
                m_editorController->jumpToLine(line);
            }
        });

    // Wire editor cursor position to status bar
    connect(
        m_editorController.get(),
        &EditorController::cursorPositionChanged,
        this,
        &MainWindow::handleCursorPositionChanged);

    // Wire language change to status bar
    connect(
        m_editorController.get(),
        &EditorController::currentLanguageChanged,
        this,
        &MainWindow::handleLanguageChanged);

    // Wire git status update to status bar
    connect(
        m_gitController.get(),
        &GitController::statusUpdated,
        this,
        &MainWindow::handleGitStatusUpdated);
}

void MainWindow::handleCursorPositionChanged(int line, int col)
{
    if (!m_cursorPosLabel)
        return;

    if (line > 0)
        m_cursorPosLabel->setText(QStringLiteral("Ln %1, Col %2").arg(line).arg(col));
    else
        m_cursorPosLabel->setText(QStringLiteral("Ln 1, Col 1"));
}

void MainWindow::handleLanguageChanged(aznora::core::Language language)
{
    if (!m_languageLabel)
        return;

    switch (language)
    {
    case aznora::core::Language::Cpp:
        m_languageLabel->setText(QStringLiteral("C++"));
        break;
    case aznora::core::Language::Python:
        m_languageLabel->setText(QStringLiteral("Python"));
        break;
    case aznora::core::Language::JavaScript:
        m_languageLabel->setText(QStringLiteral("JavaScript"));
        break;
    default:
        m_languageLabel->setText(QStringLiteral("Plain Text"));
        break;
    }
}

void MainWindow::handleGitStatusUpdated(bool clean, const QString& statusText)
{
    Q_UNUSED(clean);
    if (m_gitStatusLabel)
        m_gitStatusLabel->setText(statusText);
}

void MainWindow::setDarkTheme()
{
    if (m_darkThemeAction)
        m_darkThemeAction->setChecked(true);
    applyTheme(true);
}

void MainWindow::setLightTheme()
{
    if (m_lightThemeAction)
        m_lightThemeAction->setChecked(true);
    applyTheme(false);
}

void MainWindow::applyTheme(bool dark)
{
    if (dark)
    {
        setStyleSheet(QStringLiteral(
            "QMainWindow { background-color: #1e1e1e; color: #cccccc; }"
            "QMenuBar { background-color: #252526; color: #cccccc; border-bottom: 1px solid #333333; }"
            "QMenuBar::item { background-color: transparent; padding: 5px 10px; }"
            "QMenuBar::item:selected { background-color: #333333; color: #ffffff; }"
            "QMenu { background-color: #252526; color: #cccccc; border: 1px solid #3c3c3c; }"
            "QMenu::item { padding: 5px 25px 5px 20px; }"
            "QMenu::item:selected { background-color: #094771; color: #ffffff; }"
            "QMenu::separator { height: 1px; background-color: #333333; margin: 4px 0px; }"
            "QDockWidget { color: #cccccc; font-weight: 500; }"
            "QDockWidget::title { background-color: #252526; padding: 6px 10px; border-bottom: 1px solid #333333; font-size: 11px; font-weight: bold; text-transform: uppercase; letter-spacing: 1px; }"
            "QTreeWidget { background-color: #252526; color: #cccccc; border: none; font-size: 13px; padding: 4px; }"
            "QTreeWidget::item { padding: 4px 6px; border-radius: 3px; }"
            "QTreeWidget::item:hover { background-color: #2a2d2e; }"
            "QTreeWidget::item:selected { background-color: #094771; color: #ffffff; }"
            "QTabWidget::pane { border: none; background-color: #1e1e1e; }"
            "QTabBar::tab { background-color: #2d2d2d; color: #969696; padding: 8px 16px; border: none; border-right: 1px solid #252526; }"
            "QTabBar::tab:selected { background-color: #1e1e1e; color: #ffffff; border-top: 2px solid #007acc; }"
            "QTabBar::tab:hover:!selected { background-color: #383838; color: #cccccc; }"
            "QTextEdit#EditorArea { background-color: #1e1e1e; color: #d4d4d4; border: none; padding: 10px; selection-background-color: #264f78; selection-color: #ffffff; }"
            "QTextEdit#TerminalView { background-color: #181818; color: #4ec9b0; border: none; padding: 6px; selection-background-color: #264f78; selection-color: #ffffff; }"
            "QLineEdit#TerminalInput { background-color: #1e1e1e; color: #ffffff; border: 1px solid #3c3c3c; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton { background-color: #0e639c; color: #ffffff; border: none; border-radius: 3px; padding: 5px 12px; font-weight: 500; }"
            "QPushButton:hover { background-color: #1177bb; }"
            "QPushButton:pressed { background-color: #0d5584; }"
            "QStatusBar { background-color: #007acc; color: #ffffff; font-size: 12px; }"
            "QStatusBar QLabel { color: #ffffff; padding: 0 8px; }"
            "QScrollBar:vertical { background-color: #1e1e1e; width: 10px; margin: 0; }"
            "QScrollBar::handle:vertical { background-color: #424242; min-height: 20px; border-radius: 4px; }"
            "QScrollBar::handle:vertical:hover { background-color: #4f4f4f; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; height: 0; }"
            "QScrollBar:horizontal { background-color: #1e1e1e; height: 10px; margin: 0; }"
            "QScrollBar::handle:horizontal { background-color: #424242; min-width: 20px; border-radius: 4px; }"
            "QScrollBar::handle:horizontal:hover { background-color: #4f4f4f; }"
            "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { background: none; width: 0; }"
        ));
    }
    else
    {
        setStyleSheet(QStringLiteral(
            "QMainWindow { background-color: #f3f3f3; color: #333333; }"
            "QMenuBar { background-color: #dddddd; color: #333333; border-bottom: 1px solid #cccccc; }"
            "QMenuBar::item:selected { background-color: #cccccc; color: #000000; }"
            "QMenu { background-color: #ffffff; color: #333333; border: 1px solid #cccccc; }"
            "QMenu::item:selected { background-color: #007acc; color: #ffffff; }"
            "QDockWidget::title { background-color: #e5e5e5; padding: 6px 10px; border-bottom: 1px solid #cccccc; font-weight: bold; font-size: 11px; }"
            "QTreeWidget { background-color: #f3f3f3; color: #333333; border: none; font-size: 13px; }"
            "QTreeWidget::item:selected { background-color: #cce8ff; color: #000000; }"
            "QTabWidget::pane { border: none; background-color: #ffffff; }"
            "QTabBar::tab { background-color: #ececec; color: #666666; padding: 8px 16px; border: none; border-right: 1px solid #dddddd; }"
            "QTabBar::tab:selected { background-color: #ffffff; color: #333333; border-top: 2px solid #007acc; }"
            "QTextEdit#EditorArea { background-color: #ffffff; color: #222222; border: none; padding: 10px; selection-background-color: #add6ff; selection-color: #000000; }"
            "QTextEdit#TerminalView { background-color: #1e1e1e; color: #4ec9b0; border: none; padding: 6px; }"
            "QLineEdit#TerminalInput { background-color: #ffffff; color: #000000; border: 1px solid #cccccc; border-radius: 3px; padding: 4px 8px; }"
            "QPushButton { background-color: #007acc; color: #ffffff; border: none; border-radius: 3px; padding: 5px 12px; }"
            "QPushButton:hover { background-color: #0062a3; }"
            "QStatusBar { background-color: #007acc; color: #ffffff; font-size: 12px; }"
            "QStatusBar QLabel { color: #ffffff; padding: 0 8px; }"
        ));
    }
}

} // namespace aznora::ui
