#include "ui/SearchController.hpp"

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

    // Search bar
    auto* searchBarLayout = new QHBoxLayout();
    m_queryInput = new QLineEdit(m_containerWidget);
    m_queryInput->setPlaceholderText(QStringLiteral("Find in files..."));
    connect(m_queryInput, &QLineEdit::returnPressed, this, &SearchController::handleSearchClicked);
    searchBarLayout->addWidget(m_queryInput, 1);

    m_searchButton = new QPushButton(tr("Find"), m_containerWidget);
    connect(m_searchButton, &QPushButton::clicked, this, &SearchController::handleSearchClicked);
    searchBarLayout->addWidget(m_searchButton);

    mainLayout->addLayout(searchBarLayout);

    // Results tree
    m_resultsTree = new QTreeWidget(m_containerWidget);
    m_resultsTree->setObjectName(QStringLiteral("SearchResultsTree"));
    m_resultsTree->setHeaderLabels({tr("Results")});
    connect(m_resultsTree, &QTreeWidget::itemDoubleClicked, this, &SearchController::handleItemDoubleClicked);
    mainLayout->addWidget(m_resultsTree, 1);

    m_dockWidget->setWidget(m_containerWidget);
}

void SearchController::search(const QString& query)
{
    if (!m_resultsTree)
        return;

    m_resultsTree->clear();

    const std::string queryStr = query.trimmed().toStdString();
    if (queryStr.empty())
        return;

    const std::vector<aznora::core::SearchResult> results =
        m_searchEngine.search(m_workspacePath, queryStr);

    m_resultsTree->setHeaderLabels({
        QStringLiteral("%1 results for \"%2\"").arg(results.size()).arg(query)
    });

    // Group results by file
    std::map<std::filesystem::path, std::vector<aznora::core::SearchResult>> grouped;
    for (const auto& res : results)
    {
        grouped[res.file].push_back(res);
    }

    for (const auto& [file, fileResults] : grouped)
    {
        auto* fileItem = new QTreeWidgetItem(m_resultsTree);
        fileItem->setText(0, QString::fromStdString(file.filename().string()));
        fileItem->setIcon(0, m_resultsTree->style()->standardIcon(QStyle::SP_FileIcon));
        fileItem->setData(0, Qt::UserRole, QString::fromStdString(file.string()));
        fileItem->setData(0, Qt::UserRole + 1, -1); // Directory/file header

        for (const auto& res : fileResults)
        {
            auto* lineItem = new QTreeWidgetItem(fileItem);
            const QString snippet = QString::fromStdString(res.text).trimmed();
            lineItem->setText(0, QStringLiteral("Line %1: %2").arg(res.line).arg(snippet));
            lineItem->setData(0, Qt::UserRole, QString::fromStdString(res.file.string()));
            lineItem->setData(0, Qt::UserRole + 1, static_cast<int>(res.line));
        }

        fileItem->setExpanded(true);
    }
}

void SearchController::handleSearchClicked()
{
    if (!m_queryInput)
        return;

    search(m_queryInput->text());
}

void SearchController::handleItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if (!item)
        return;

    const QString pathStr = item->data(0, Qt::UserRole).toString();
    const int line = item->data(0, Qt::UserRole + 1).toInt();

    if (pathStr.isEmpty())
        return;

    emit resultSelected(std::filesystem::path(pathStr.toStdString()), line > 0 ? line : 1);
}

} // namespace aznora::ui
