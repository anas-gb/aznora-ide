#include "ui/TerminalController.hpp"

#include <QDockWidget>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QVBoxLayout>

namespace aznora::ui {

TerminalController::TerminalController(
    QDockWidget* dockWidget,
    aznora::core::TerminalService& terminalService,
    QObject* parent)
    : QObject(parent)
    , m_dockWidget(dockWidget)
    , m_terminalService(terminalService)
{
    setupUi();
}

void TerminalController::setupUi()
{
    if (!m_dockWidget)
        return;

    m_containerWidget = new QWidget(m_dockWidget);
    auto* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Output area
    m_outputLog = new QTextEdit(m_containerWidget);
    m_outputLog->setObjectName(QStringLiteral("TerminalView"));
    m_outputLog->setReadOnly(true);

    QFont terminalFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    terminalFont.setPointSize(10);
    m_outputLog->setFont(terminalFont);
    m_outputLog->setPlainText(QStringLiteral("Aznora Terminal Ready\n"));
    mainLayout->addWidget(m_outputLog, 1);

    // Input bar layout
    auto* inputLayout = new QHBoxLayout();
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(4);

    m_commandInput = new QLineEdit(m_containerWidget);
    m_commandInput->setObjectName(QStringLiteral("TerminalInput"));
    m_commandInput->setFont(terminalFont);
    m_commandInput->setPlaceholderText(QStringLiteral("Type command here (e.g., cmake --build build, ls, git status)..."));
    connect(m_commandInput, &QLineEdit::returnPressed, this, &TerminalController::handleInputReturnPressed);
    inputLayout->addWidget(m_commandInput, 1);

    m_runButton = new QPushButton(tr("Run"), m_containerWidget);
    connect(m_runButton, &QPushButton::clicked, this, &TerminalController::handleRunClicked);
    inputLayout->addWidget(m_runButton);

    m_clearButton = new QPushButton(tr("Clear"), m_containerWidget);
    connect(m_clearButton, &QPushButton::clicked, this, &TerminalController::clear);
    inputLayout->addWidget(m_clearButton);

    mainLayout->addLayout(inputLayout);

    m_dockWidget->setWidget(m_containerWidget);
}

void TerminalController::executeCommand(const QString& command)
{
    const QString trimmed = command.trimmed();
    if (trimmed.isEmpty())
        return;

    m_history.append(trimmed);
    m_historyIndex = m_history.size();

    appendOutput(QStringLiteral("$ ") + trimmed + QStringLiteral("\n"));

    const aznora::core::CommandResult result =
        m_terminalService.execute(trimmed.toStdString());

    if (!result.output.empty())
    {
        appendOutput(QString::fromStdString(result.output));
        if (result.output.back() != '\n')
            appendOutput(QStringLiteral("\n"));
    }

    if (result.exitCode != 0)
    {
        appendOutput(QStringLiteral("[Exit code: %1]\n").arg(result.exitCode));
    }

    emit commandFinished(result.exitCode, QString::fromStdString(result.output));
}

void TerminalController::appendOutput(const QString& text)
{
    if (!m_outputLog)
        return;

    m_outputLog->moveCursor(QTextCursor::End);
    m_outputLog->insertPlainText(text);
    m_outputLog->moveCursor(QTextCursor::End);

    if (QScrollBar* scroll = m_outputLog->verticalScrollBar())
        scroll->setValue(scroll->maximum());
}

void TerminalController::clear()
{
    if (!m_outputLog)
        return;

    m_outputLog->clear();
    m_outputLog->setPlainText(QStringLiteral("Aznora Terminal Ready\n"));
}

void TerminalController::handleRunClicked()
{
    if (!m_commandInput)
        return;

    const QString cmd = m_commandInput->text();
    m_commandInput->clear();
    executeCommand(cmd);
}

void TerminalController::handleInputReturnPressed()
{
    handleRunClicked();
}

} // namespace aznora::ui
