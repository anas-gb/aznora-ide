#pragma once

#include <QObject>
#include <filesystem>
#include <vector>

#include "core/search/SearchEngine.hpp"

class QDockWidget;
class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QWidget;

namespace aznora::ui {

class SearchController : public QObject
{
    Q_OBJECT

public:
    SearchController(
        QDockWidget* dockWidget,
        aznora::core::SearchEngine& searchEngine,
        QObject* parent = nullptr);

    ~SearchController() override = default;

    void setWorkspacePath(const std::filesystem::path& path);
    [[nodiscard]] std::filesystem::path workspacePath() const;

public slots:
    void search(const QString& query);

signals:
    void resultSelected(const std::filesystem::path& filePath, int line);

private slots:
    void handleSearchClicked();
    void handleItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    void setupUi();

    QDockWidget* m_dockWidget{nullptr};
    aznora::core::SearchEngine& m_searchEngine;
    std::filesystem::path m_workspacePath;

    QWidget* m_containerWidget{nullptr};
    QLineEdit* m_queryInput{nullptr};
    QPushButton* m_searchButton{nullptr};
    QTreeWidget* m_resultsTree{nullptr};
};

} // namespace aznora::ui
