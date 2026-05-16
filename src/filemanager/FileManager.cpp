/**
 * NanoMark — FileManager.cpp
 * File I/O implementation
 */

#include "FileManager.h"
#include "Logger.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>

namespace NanoMark {

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
}

FileResult FileManager::readFile(const QString &filePath)
{
    FileResult result;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.success = false;
        result.errorMessage = file.errorString();
        Logger::instance().error("Failed to open file: " + filePath + " - " + result.errorMessage);
        return result;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    result.content = stream.readAll();
    result.success = true;

    file.close();
    Logger::instance().info("Read file: " + filePath);
    return result;
}

FileResult FileManager::writeFile(const QString &filePath, const QString &content)
{
    FileResult result;

    // Write to a temporary file first, then rename (atomic save)
    QString tempPath = filePath + ".tmp";
    QFile tempFile(tempPath);

    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        result.success = false;
        result.errorMessage = tempFile.errorString();
        Logger::instance().error("Failed to write file: " + filePath + " - " + result.errorMessage);
        return result;
    }

    QTextStream stream(&tempFile);
    stream.setEncoding(QStringConverter::Utf8);
    stream << content;
    tempFile.close();

    // Remove existing file and rename temp
    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
    }

    if (!QFile::rename(tempPath, filePath)) {
        result.success = false;
        result.errorMessage = "Failed to rename temporary file.";
        Logger::instance().error(result.errorMessage);
        return result;
    }

    result.success = true;
    Logger::instance().info("Saved file: " + filePath);
    return result;
}

QStringList FileManager::recentFiles() const
{
    QSettings settings;
    return settings.value("recentFiles").toStringList();
}

void FileManager::addRecentFile(const QString &filePath)
{
    QSettings settings;
    QStringList recent = settings.value("recentFiles").toStringList();

    recent.removeAll(filePath);
    recent.prepend(filePath);

    while (recent.size() > MAX_RECENT_FILES) {
        recent.removeLast();
    }

    settings.setValue("recentFiles", recent);
}

void FileManager::clearRecentFiles()
{
    QSettings settings;
    settings.remove("recentFiles");
}

} // namespace NanoMark
