/**
 * NanoMark — WorkspaceManager.cpp
 * Workspace directory management
 */

#include "WorkspaceManager.h"

#include <QFileSystemModel>
#include <QDir>

namespace NanoMark {

WorkspaceManager::WorkspaceManager(QObject *parent)
    : QObject(parent)
    , m_fsModel(new QFileSystemModel(this))
{
    m_fsModel->setRootPath(QDir::homePath());
    m_fsModel->setNameFilters({"*.md", "*.markdown", "*.txt", "*.html", "*.css", "*.json"});
    m_fsModel->setNameFilterDisables(false);
}

void WorkspaceManager::openWorkspace(const QString &dirPath)
{
    m_workspacePath = dirPath;
    m_fsModel->setRootPath(dirPath);
    emit workspaceChanged(dirPath);
}

QString WorkspaceManager::currentWorkspace() const
{
    return m_workspacePath;
}

QFileSystemModel* WorkspaceManager::fileSystemModel() const
{
    return m_fsModel;
}

} // namespace NanoMark
