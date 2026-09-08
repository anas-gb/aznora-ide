#pragma once

#include <QMainWindow>

class QDockWidget;
class QMenuBar;
class QTextEdit;
class QTreeWidget;

namespace aznora::ui {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUi();
    void setupMenuBar();
    void setupExplorerDock();
    void setupEditorArea();
    void setupTerminalDock();
    void applyTheme();

    QDockWidget* m_explorerDock{nullptr};
    QDockWidget* m_terminalDock{nullptr};
    QTreeWidget* m_explorerTree{nullptr};
    QTextEdit* m_editor{nullptr};
    QTextEdit* m_terminalView{nullptr};
};

} // namespace aznora::ui
