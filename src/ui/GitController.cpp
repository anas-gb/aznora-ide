#include "ui/GitController.hpp"

#include <QDockWidget>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace aznora::ui {

GitController::GitController(
    QDockWidget* dockWidget,
    aznora::core::GitService& gitService,
    QObject* parent)
    : QObject(parent)
    , m_dockWidget(dockWidget)
    , m_gitService(gitService)
    , m_workspacePath(std::filesystem::current_path())
{
    setupUi();
    refreshStatus();
}

void GitController::setWorkspacePath(const std::filesystem::path& path)
{
    m_workspacePath = path;
    refreshStatus();
}

std::filesystem::path GitController::workspacePath() const
{
    return m_workspacePath;
}

void GitController::setupUi()
{
    if (!m_dockWidget)
        return;

    m_containerWidget = new QWidget(m_dockWidget);
    auto* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Toolbar (Refresh, Pull, Push)
    auto* toolbarLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton(tr("Refresh"), m_containerWidget);
    connect(m_refreshButton, &QPushButton::clicked, this, &GitController::refreshStatus);
    toolbarLayout->addWidget(m_refreshButton);

    m_pullButton = new QPushButton(tr("Pull"), m_containerWidget);
    connect(m_pullButton, &QPushButton::clicked, this, &GitController::pull);
    toolbarLayout->addWidget(m_pullButton);

    m_pushButton = new QPushButton(tr("Push"), m_containerWidget);
    connect(m_pushButton, &QPushButton::clicked, this, &GitController::push);
    toolbarLayout->addWidget(m_pushButton);

    mainLayout->addLayout(toolbarLayout);

    // Status view
    m_statusDisplay = new QTextEdit(m_containerWidget);
    m_statusDisplay->setObjectName(QStringLiteral("GitStatusDisplay"));
    m_statusDisplay->setReadOnly(true);
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(10);
    m_statusDisplay->setFont(monoFont);
    mainLayout->addWidget(m_statusDisplay, 1);

    // Commit section
    auto* commitLayout = new QVBoxLayout();
    m_commitInput = new QLineEdit(m_containerWidget);
    m_commitInput->setPlaceholderText(QStringLiteral("Commit message..."));
    connect(m_commitInput, &QLineEdit::returnPressed, this, &GitController::commit);
    commitLayout->addWidget(m_commitInput);

    m_commitButton = new QPushButton(tr("Commit"), m_containerWidget);
    connect(m_commitButton, &QPushButton::clicked, this, &GitController::commit);
    commitLayout->addWidget(m_commitButton);

    mainLayout->addLayout(commitLayout);

    m_dockWidget->setWidget(m_containerWidget);
}

void GitController::refreshStatus()
{
    if (!m_statusDisplay)
        return;

    const aznora::core::GitStatus status = m_gitService.status(m_workspacePath);

    if (!status.success)
    {
        m_statusDisplay->setPlainText(
            QStringLiteral("Git repository not initialized or error checking status.\n") +
            QString::fromStdString(status.output));
        emit statusUpdated(false, QStringLiteral("Git: No repo"));
        return;
    }

    m_statusDisplay->setPlainText(QString::fromStdString(status.output));

    const QString summary = status.clean ? QStringLiteral("Git: Clean") : QStringLiteral("Git: Modified");
    emit statusUpdated(status.clean, summary);
}

void GitController::commit()
{
    if (!m_commitInput)
        return;

    const QString message = m_commitInput->text().trimmed();
    if (message.isEmpty())
        return;

    const bool ok = m_gitService.commit(m_workspacePath, message.toStdString());
    if (ok)
    {
        m_commitInput->clear();
    }
    refreshStatus();
}

void GitController::pull()
{
    m_gitService.pull(m_workspacePath);
    refreshStatus();
}

void GitController::push()
{
    m_gitService.push(m_workspacePath);
    refreshStatus();
}

} // namespace aznora::ui
