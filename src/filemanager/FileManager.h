#pragma once

/**
 * NanoMark — FileManager.h
 * File I/O, recent files, auto-save
 */

#include <QObject>
#include <QString>
#include <QStringList>

namespace NanoMark {

struct FileResult {
    bool success = false;
    QString content;
    QString errorMessage;
};

class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);

    /** Read file contents */
    FileResult readFile(const QString &filePath);

    /** Write content to file */
    FileResult writeFile(const QString &filePath, const QString &content);

    /** Get recent files list */
    QStringList recentFiles() const;

    /** Add a file to recent files */
    void addRecentFile(const QString &filePath);

    /** Clear recent files */
    void clearRecentFiles();

private:
    static constexpr int MAX_RECENT_FILES = 15;
};

} // namespace NanoMark
