#include "ui/BuildController.hpp"
#include "ui/TerminalController.hpp"

namespace aznora::ui {

BuildController::BuildController(
    aznora::core::BuildService& buildService,
    TerminalController& terminalController,
    QObject* parent)
    : QObject(parent)
    , m_buildService(buildService)
    , m_terminalController(terminalController)
    , m_workspacePath(std::filesystem::current_path())
{
}

void BuildController::setWorkspacePath(const std::filesystem::path& path)
{
    m_workspacePath = path;
}

std::filesystem::path BuildController::workspacePath() const
{
    return m_workspacePath;
}

void BuildController::buildProject()
{
    const QString projectName = QString::fromStdString(m_workspacePath.filename().string());
    m_terminalController.appendOutput(
        QStringLiteral("\n=== Building project: %1 ===\n").arg(projectName));

    const aznora::core::BuildResult result =
        m_buildService.buildProject(m_workspacePath);

    if (!result.output.empty())
        m_terminalController.appendOutput(QString::fromStdString(result.output));

    if (result.success)
    {
        m_terminalController.appendOutput(
            QStringLiteral("=== Build succeeded ===\n"));
    }
    else
    {
        m_terminalController.appendOutput(
            QStringLiteral("=== Build failed (exit code %1) ===\n").arg(result.exitCode));
    }

    emit buildFinished(result.success, result.exitCode);
}

void BuildController::runProject()
{
    const QString projectName = QString::fromStdString(m_workspacePath.filename().string());
    m_terminalController.appendOutput(
        QStringLiteral("\n=== Running project: %1 ===\n").arg(projectName));

    const aznora::core::BuildResult result =
        m_buildService.runProject(m_workspacePath);

    if (!result.output.empty())
        m_terminalController.appendOutput(QString::fromStdString(result.output));

    if (result.success)
    {
        m_terminalController.appendOutput(
            QStringLiteral("=== Process exited normally ===\n"));
    }
    else
    {
        m_terminalController.appendOutput(
            QStringLiteral("=== Process failed (exit code %1) ===\n").arg(result.exitCode));
    }

    emit runFinished(result.success, result.exitCode);
}

} // namespace aznora::ui
