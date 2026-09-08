#include "ui/WorkspaceTreeController.hpp"

#include <QApplication>
#include <QStyle>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <algorithm>
#include <iterator>
#include <system_error>

namespace aznora::ui {

WorkspaceTreeController::WorkspaceTreeController(QTreeWidget* treeWidget, QObject* parent)
    : QObject(parent)
    , m_treeWidget(treeWidget)
{
    if (m_treeWidget)
    {
        connect(
            m_treeWidget,
            &QTreeWidget::itemDoubleClicked,
            this,
            &WorkspaceTreeController::handleItemDoubleClicked);
    }
}

void WorkspaceTreeController::loadWorkspace(const std::filesystem::path& workspacePath)
{
    if (!m_treeWidget)
        return;

    std::error_code ec;
    if (!std::filesystem::exists(workspacePath, ec) || ec)
        return;

    const std::filesystem::path canonicalPath =
        std::filesystem::weakly_canonical(workspacePath, ec);
    const std::filesystem::path rootPath = ec ? workspacePath : canonicalPath;

    std::string rootName = rootPath.filename().string();
    if (rootName.empty())
        rootName = rootPath.string();

    TreeNode rootNode;
    rootNode.name = rootName;
    rootNode.path = rootPath;
    rootNode.isDirectory = true;

    const std::vector<aznora::core::FileEntry> entries =
        m_fileExplorer.listFiles(rootPath);

    buildTree(rootNode, rootPath, entries);
    sortTree(rootNode);

    m_treeWidget->clear();

    auto* rootItem = new QTreeWidgetItem(
        m_treeWidget,
        QStringList{QString::fromStdString(rootNode.name)});
    rootItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));
    rootItem->setData(0, Qt::UserRole, QString::fromStdString(rootNode.path.string()));
    rootItem->setData(0, Qt::UserRole + 1, true);

    populateTreeWidget(rootNode, rootItem);

    rootItem->setExpanded(true);
}

void WorkspaceTreeController::buildTree(
    TreeNode& rootNode,
    const std::filesystem::path& rootPath,
    const std::vector<aznora::core::FileEntry>& entries)
{
    for (const auto& entry : entries)
    {
        std::error_code ec;
        const std::filesystem::path rel =
            std::filesystem::relative(entry.path, rootPath, ec);

        if (ec || rel.empty())
            continue;

        TreeNode* current = &rootNode;
        std::filesystem::path accumulated = rootPath;

        auto it = rel.begin();
        const auto end = rel.end();

        for (; it != end; ++it)
        {
            const std::string partName = it->string();
            accumulated /= *it;
            const bool isLast = (std::next(it) == end);
            const bool partIsDir = isLast ? entry.isDirectory : true;

            auto childIt = std::find_if(
                current->children.begin(),
                current->children.end(),
                [&partName](const std::unique_ptr<TreeNode>& child) {
                    return child->name == partName;
                });

            if (childIt != current->children.end())
            {
                current = childIt->get();
                if (isLast)
                {
                    current->isDirectory = entry.isDirectory;
                    current->path = entry.path;
                }
            }
            else
            {
                auto newChild = std::make_unique<TreeNode>();
                newChild->name = partName;
                newChild->path = isLast ? entry.path : accumulated;
                newChild->isDirectory = partIsDir;

                TreeNode* nextCurrent = newChild.get();
                current->children.push_back(std::move(newChild));
                current = nextCurrent;
            }
        }
    }
}

void WorkspaceTreeController::sortTree(TreeNode& node)
{
    std::sort(
        node.children.begin(),
        node.children.end(),
        [](const std::unique_ptr<TreeNode>& left, const std::unique_ptr<TreeNode>& right) {
            if (left->isDirectory != right->isDirectory)
                return left->isDirectory > right->isDirectory;

            return left->name < right->name;
        });

    for (const auto& child : node.children)
    {
        if (child->isDirectory)
            sortTree(*child);
    }
}

void WorkspaceTreeController::populateTreeWidget(
    const TreeNode& node,
    QTreeWidgetItem* parentItem)
{
    for (const auto& child : node.children)
    {
        auto* childItem = new QTreeWidgetItem(
            parentItem,
            QStringList{QString::fromStdString(child->name)});

        const QStyle::StandardPixmap iconType =
            child->isDirectory ? QStyle::SP_DirIcon : QStyle::SP_FileIcon;
        childItem->setIcon(0, m_treeWidget->style()->standardIcon(iconType));

        childItem->setData(0, Qt::UserRole, QString::fromStdString(child->path.string()));
        childItem->setData(0, Qt::UserRole + 1, child->isDirectory);

        if (child->isDirectory)
        {
            populateTreeWidget(*child, childItem);
        }
    }
}

void WorkspaceTreeController::handleItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if (!item)
        return;

    const bool isDirectory = item->data(0, Qt::UserRole + 1).toBool();
    if (isDirectory)
        return;

    const QString pathString = item->data(0, Qt::UserRole).toString();
    if (pathString.isEmpty())
        return;

    emit fileDoubleClicked(std::filesystem::path(pathString.toStdString()));
}

} // namespace aznora::ui
