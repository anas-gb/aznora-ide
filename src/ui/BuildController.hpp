#pragma once

#include <QObject>
#include <filesystem>

#include "core/build/BuildService.hpp"

namespace aznora::ui {

class TerminalController;

class BuildController : public QObject
{
    Q_OBJECT

public:
    BuildController(
        aznora::core::BuildService& buildService,
        TerminalController& terminalController,
        QObject* parent = nullptr);

    ~BuildController() override = default;

    void setWorkspacePath(const std::filesystem::path& path);
    [[nodiscard]] std::filesystem::path workspacePath() const;

public slots:
    void buildProject();
    void runProject();

signals:
    void buildFinished(bool success, int exitCode);
    void runFinished(bool success, int exitCode);

private:
    aznora::core::BuildService& m_buildService;
    TerminalController& m_terminalController;
    std::filesystem::path m_workspacePath;
};

} // namespace aznora::ui
