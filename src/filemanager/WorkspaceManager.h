#pragma once

/**
 * NanoMark — WorkspaceManager.h
 * Workspace/folder management
 */

#include <QObject>
#include <QString>

class QFileSystemModel;

namespace NanoMark {

class WorkspaceManager : public QObject
{
    Q_OBJECT

public:
    explicit WorkspaceManager(QObject *parent = nullptr);

    /** Open a workspace directory */
    void openWorkspace(const QString &dirPath);

    /** Get the current workspace path */
    QString currentWorkspace() const;

    /** Get the file system model */
    QFileSystemModel* fileSystemModel() const;

signals:
    void workspaceChanged(const QString &path);

private:
    QString m_workspacePath;
    QFileSystemModel *m_fsModel = nullptr;
};

} // namespace NanoMark
