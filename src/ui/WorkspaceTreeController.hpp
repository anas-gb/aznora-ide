#pragma once

#include <QObject>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/filesystem/FileExplorer.hpp"

class QTreeWidget;
class QTreeWidgetItem;

namespace aznora::ui {

class WorkspaceTreeController : public QObject
{
    Q_OBJECT

public:
    explicit WorkspaceTreeController(QTreeWidget* treeWidget, QObject* parent = nullptr);
    ~WorkspaceTreeController() override = default;

    void loadWorkspace(const std::filesystem::path& workspacePath);

private:
    struct TreeNode
    {
        std::string name;
        std::filesystem::path path;
        bool isDirectory{false};
        std::vector<std::unique_ptr<TreeNode>> children;
    };

    void buildTree(
        TreeNode& rootNode,
        const std::filesystem::path& rootPath,
        const std::vector<aznora::core::FileEntry>& entries);

    void sortTree(TreeNode& node);

    void populateTreeWidget(
        const TreeNode& node,
        QTreeWidgetItem* parentItem);

    QTreeWidget* m_treeWidget{nullptr};
    aznora::core::FileExplorer m_fileExplorer;

signals:
    void fileDoubleClicked(const std::filesystem::path& filePath);

private slots:
    void handleItemDoubleClicked(QTreeWidgetItem* item, int column);
};

} // namespace aznora::ui
