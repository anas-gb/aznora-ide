#pragma once

#include <QObject>
#include <filesystem>

#include "core/git/GitService.hpp"

class QDockWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QWidget;

namespace aznora::ui {

class GitController : public QObject
{
    Q_OBJECT

public:
    GitController(
        QDockWidget* dockWidget,
        aznora::core::GitService& gitService,
        QObject* parent = nullptr);

    ~GitController() override = default;

    void setWorkspacePath(const std::filesystem::path& path);
    [[nodiscard]] std::filesystem::path workspacePath() const;

public slots:
    void refreshStatus();
    void commit();
    void pull();
    void push();

signals:
    void statusUpdated(bool clean, const QString& statusText);

private:
    void setupUi();

    QDockWidget* m_dockWidget{nullptr};
    aznora::core::GitService& m_gitService;
    std::filesystem::path m_workspacePath;

    QWidget* m_containerWidget{nullptr};
    QTextEdit* m_statusDisplay{nullptr};
    QLineEdit* m_commitInput{nullptr};
    QPushButton* m_commitButton{nullptr};
    QPushButton* m_refreshButton{nullptr};
    QPushButton* m_pullButton{nullptr};
    QPushButton* m_pushButton{nullptr};
};

} // namespace aznora::ui
