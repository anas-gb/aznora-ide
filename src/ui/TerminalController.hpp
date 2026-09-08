#pragma once

#include <QObject>
#include <QStringList>
#include <memory>

#include "core/terminal/TerminalService.hpp"

class QDockWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QWidget;

namespace aznora::ui {

class TerminalController : public QObject
{
    Q_OBJECT

public:
    TerminalController(
        QDockWidget* dockWidget,
        aznora::core::TerminalService& terminalService,
        QObject* parent = nullptr);

    ~TerminalController() override = default;

    void appendOutput(const QString& text);
    void clear();

public slots:
    void executeCommand(const QString& command);

signals:
    void commandFinished(int exitCode, const QString& output);

private slots:
    void handleRunClicked();
    void handleInputReturnPressed();

private:
    void setupUi();

    QDockWidget* m_dockWidget{nullptr};
    aznora::core::TerminalService& m_terminalService;

    QWidget* m_containerWidget{nullptr};
    QTextEdit* m_outputLog{nullptr};
    QLineEdit* m_commandInput{nullptr};
    QPushButton* m_runButton{nullptr};
    QPushButton* m_clearButton{nullptr};

    QStringList m_history;
    int m_historyIndex{-1};
};

} // namespace aznora::ui
