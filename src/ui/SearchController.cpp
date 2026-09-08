#include "ui/SearchController.hpp"

#include <QDebug>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <map>

namespace aznora::ui {

SearchController::SearchController(
    QDockWidget* dockWidget,
    aznora::core::SearchEngine& searchEngine,
    QObject* parent)
    : QObject(parent)
    , m_dockWidget(dockWidget)
    , m_searchEngine(searchEngine)
    , m_workspacePath(std::filesystem::current_path())
{
    setupUi();
}

void SearchController::setWorkspacePath(const std::filesystem::path& path)
{
    m_workspacePath = path;

    qDebug() << "=== WORKSPACE UPDATED ===";
    qDebug() << "Workspace:"
             << QString::fromStdString(m_workspacePath.string());
}

std::filesystem::path SearchController::workspacePath() const
{
    return m_workspacePath;
}

void SearchController::setupUi()
{
    if (!m_dockWidget)
        return;

    m_containerWidget = new QWidget(m_dockWidget);

    auto* mainLayout = new QVBoxLayout(m_containerWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    auto* searchBarLayout = new QHBoxLayout();

    m_queryInput = new QLineEdit(m_containerWidget);
    m_queryInput->setPlaceholderText(QStringLiteral("Find in files..."));

    connect(
        m_queryInput,
        &QLineEdit::returnPressed,
        this,
        &SearchController::handleSearchClicked);

    searchBarLayout->addWidget(m_queryInput, 1);

    m_searchButton = new QPushButton(tr("Find"), m_containerWidget);

    connect(
        m_searchButton,
        &QPushButton::clicked,
        this,
        &SearchController::handleSearchClicked);

    searchBarLayout->addWidget(m_searchButton);

    mainLayout->addLayout(searchBarLayout);

    m_resultsTree = new QTreeWidget(m_containerWidget);
    m_resultsTree->setObjectName(QStringLiteral("SearchResultsTree"));
    m_resultsTree->setHeaderLabels({QStringLiteral("Results")});

    connect(
        m_resultsTree,
        &QTreeWidget::itemDoubleClicked,
        this,
        &SearchController::handleItemDoubleClicked);

    mainLayout->addWidget(m_resultsTree, 1);

    m_dockWidget->setWidget(m_containerWidget);
}

void SearchController::search(const QString& query)
{
    if (!m_resultsTree)
        return;

    m_resultsTree->clear();

    const QString trimmedQuery = query.trimmed();

    if (trimmedQuery.isEmpty())
    {
        qDebug() << "Search query is empty.";
        return;
    }

    qDebug() << "";
    qDebug() << "========== SEARCH ==========";
    qDebug() << "Query:"
             << trimmedQuery;
    qDebug() << "Workspace:"
             << QString::fromStdString(m_workspacePath.string());

    const std::vector<aznora::core::SearchResult> results =
        m_searchEngine.search(
            m_workspacePath,
            trimmedQuery.toStdString());

    qDebug() << "Results found:"
             << results.size();

    m_resultsTree->setHeaderLabels({
        QStringLiteral("%1 results for \"%2\"")
            .arg(results.size())
            .arg(trimmedQuery)
    });

    std::map<
        std::filesystem::path,
        std::vector<aznora::core::SearchResult>> grouped;

    for (const auto& result : results)
    {
        grouped[result.file].push_back(result);
    }

    for (const auto& [file, fileResults] : grouped)
    {
        auto* fileItem = new QTreeWidgetItem(m_resultsTree);

        fileItem->setText(
            0,
            QString::fromStdString(
                file.filename().string()));

        fileItem->setIcon(
            0,
            m_resultsTree->style()->standardIcon(
                QStyle::SP_FileIcon));

        fileItem->setData(
            0,
            Qt::UserRole,
            QString::fromStdString(file.string()));

        fileItem->setData(
            0,
            Qt::UserRole + 1,
            -1);

        for (const auto& result : fileResults)
        {
            auto* lineItem =
                new QTreeWidgetItem(fileItem);

            lineItem->setText(
                0,
                QStringLiteral("Line %1: %2")
                    .arg(result.line)
                    .arg(QString::fromStdString(result.text).trimmed()));

            lineItem->setData(
                0,
                Qt::UserRole,
                QString::fromStdString(
                    result.file.string()));

            lineItem->setData(
                0,
                Qt::UserRole + 1,
                static_cast<int>(result.line));
        }

        fileItem->setExpanded(true);
    }

    qDebug() << "============================";
}

void SearchController::handleSearchClicked()
{
    if (!m_queryInput)
        return;

    qDebug() << "";
    qDebug() << "=== SEARCH BUTTON CLICKED ===";
    qDebug() << "Text:"
             << m_queryInput->text();

    search(m_queryInput->text());
}

void SearchController::handleItemDoubleClicked(
    QTreeWidgetItem* item,
    int column)
{
    Q_UNUSED(column);

    if (!item)
        return;

    const QString pathStr =
        item->data(0, Qt::UserRole).toString();

    const int line =
        item->data(0, Qt::UserRole + 1).toInt();

    if (pathStr.isEmpty())
        return;

    qDebug() << "Opening search result:";
    qDebug() << pathStr;
    qDebug() << "Line:" << line;

    emit resultSelected(
        std::filesystem::path(pathStr.toStdString()),
        line > 0 ? line : 1);
}

} // namespace aznora::ui