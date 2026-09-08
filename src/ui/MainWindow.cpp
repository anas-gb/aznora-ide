#include "ui/MainWindow.hpp"

#include <QAction>
#include <QDockWidget>
#include <QFont>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace aznora::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Aznora IDE"));
    resize(1400, 900);

    setupUi();
    applyTheme();
}

void MainWindow::setupUi()
{
    setupMenuBar();
    setupExplorerDock();
    setupEditorArea();
    setupTerminalDock();

    // Attach dock toggle actions into the View menu
    if (QMenuBar* menu = menuBar()) {
        for (QAction* action : menu->actions()) {
            if (action->text() == tr("&View") && action->menu()) {
                QMenu* viewMenu = action->menu();
                viewMenu->addAction(m_explorerDock->toggleViewAction());
                viewMenu->addAction(m_terminalDock->toggleViewAction());
                break;
            }
        }
    }

    // Set proportional initial layout sizing
    resizeDocks({m_explorerDock}, {280}, Qt::Horizontal);
    resizeDocks({m_terminalDock}, {220}, Qt::Vertical);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menu = menuBar();

    // File Menu
    QMenu* fileMenu = menu->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New File"), QStringLiteral("Ctrl+N"));
    fileMenu->addAction(tr("&Open File..."), QStringLiteral("Ctrl+O"));
    fileMenu->addAction(tr("Open &Folder..."));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Save"), QStringLiteral("Ctrl+S"));
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

    // View Menu
    menu->addMenu(tr("&View"));

    // Git Menu
    QMenu* gitMenu = menu->addMenu(tr("&Git"));
    gitMenu->addAction(tr("&Status"));
    gitMenu->addAction(tr("&Commit..."));
    gitMenu->addAction(tr("&Push"));
    gitMenu->addAction(tr("P&ull"));

    // Help Menu
    QMenu* helpMenu = menu->addMenu(tr("&Help"));
    QAction* aboutAction = helpMenu->addAction(tr("&About Aznora IDE"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(
            this,
            tr("About Aznora IDE"),
            tr("<h2>Aznora IDE</h2>"
               "<p>Version 0.1.0</p>"
               "<p>A native C++20 desktop IDE built with Qt6.</p>"));
    });
}

void MainWindow::setupExplorerDock()
{
    m_explorerDock = new QDockWidget(tr("Explorer"), this);
    m_explorerDock->setObjectName(QStringLiteral("ExplorerDock"));
    m_explorerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_explorerTree = new QTreeWidget(m_explorerDock);
    m_explorerTree->setObjectName(QStringLiteral("ExplorerTree"));
    m_explorerTree->setHeaderHidden(true);

    auto* workspaceItem = new QTreeWidgetItem(
        m_explorerTree,
        QStringList{QStringLiteral("Workspace")});

    new QTreeWidgetItem(workspaceItem, QStringList{QStringLiteral("src")});
    new QTreeWidgetItem(workspaceItem, QStringList{QStringLiteral("README.md")});

    workspaceItem->setExpanded(true);

    m_explorerDock->setWidget(m_explorerTree);
    addDockWidget(Qt::LeftDockWidgetArea, m_explorerDock);
}

void MainWindow::setupEditorArea()
{
    m_editor = new QTextEdit(this);
    m_editor->setObjectName(QStringLiteral("EditorArea"));
    m_editor->setPlainText(QStringLiteral("Welcome to Aznora IDE"));

    QFont editorFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    editorFont.setPointSize(12);
    m_editor->setFont(editorFont);

    setCentralWidget(m_editor);
}

void MainWindow::setupTerminalDock()
{
    m_terminalDock = new QDockWidget(tr("Terminal"), this);
    m_terminalDock->setObjectName(QStringLiteral("TerminalDock"));
    m_terminalDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    m_terminalView = new QTextEdit(m_terminalDock);
    m_terminalView->setObjectName(QStringLiteral("TerminalView"));
    m_terminalView->setReadOnly(true);
    m_terminalView->setPlainText(QStringLiteral("Aznora Terminal Ready"));

    QFont terminalFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    terminalFont.setPointSize(11);
    m_terminalView->setFont(terminalFont);

    m_terminalDock->setWidget(m_terminalView);
    addDockWidget(Qt::BottomDockWidgetArea, m_terminalDock);
}

void MainWindow::applyTheme()
{
    setStyleSheet(QStringLiteral(
        "QMainWindow {"
        "    background-color: #1e1e1e;"
        "}"
        "QMenuBar {"
        "    background-color: #252526;"
        "    color: #cccccc;"
        "    border-bottom: 1px solid #333333;"
        "}"
        "QMenuBar::item {"
        "    background-color: transparent;"
        "    padding: 5px 10px;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #333333;"
        "    color: #ffffff;"
        "}"
        "QMenu {"
        "    background-color: #252526;"
        "    color: #cccccc;"
        "    border: 1px solid #3c3c3c;"
        "}"
        "QMenu::item {"
        "    padding: 5px 25px 5px 20px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #094771;"
        "    color: #ffffff;"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background-color: #333333;"
        "    margin: 4px 0px;"
        "}"
        "QDockWidget {"
        "    color: #cccccc;"
        "    font-weight: 500;"
        "}"
        "QDockWidget::title {"
        "    background-color: #252526;"
        "    padding: 6px 10px;"
        "    border-bottom: 1px solid #333333;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    text-transform: uppercase;"
        "    letter-spacing: 1px;"
        "}"
        "QTreeWidget#ExplorerTree {"
        "    background-color: #252526;"
        "    color: #cccccc;"
        "    border: none;"
        "    font-size: 13px;"
        "    padding: 4px;"
        "}"
        "QTreeWidget#ExplorerTree::item {"
        "    padding: 4px 6px;"
        "    border-radius: 3px;"
        "}"
        "QTreeWidget#ExplorerTree::item:hover {"
        "    background-color: #2a2d2e;"
        "}"
        "QTreeWidget#ExplorerTree::item:selected {"
        "    background-color: #094771;"
        "    color: #ffffff;"
        "}"
        "QTextEdit#EditorArea {"
        "    background-color: #1e1e1e;"
        "    color: #d4d4d4;"
        "    border: none;"
        "    padding: 10px;"
        "    selection-background-color: #264f78;"
        "    selection-color: #ffffff;"
        "}"
        "QTextEdit#TerminalView {"
        "    background-color: #181818;"
        "    color: #4ec9b0;"
        "    border: none;"
        "    padding: 8px;"
        "    selection-background-color: #264f78;"
        "    selection-color: #ffffff;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #1e1e1e;"
        "    width: 10px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #424242;"
        "    min-height: 20px;"
        "    border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #4f4f4f;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    background: none;"
        "    height: 0px;"
        "}"
        "QScrollBar:horizontal {"
        "    background-color: #1e1e1e;"
        "    height: 10px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "    background-color: #424242;"
        "    min-width: 20px;"
        "    border-radius: 4px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "    background-color: #4f4f4f;"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "    background: none;"
        "    width: 0px;"
        "}"
    ));
}

} // namespace aznora::ui
