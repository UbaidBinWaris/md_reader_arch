/**
 * NanoMark — AutosaveManager.cpp
 * Background autosave implementation
 */

#include "AutosaveManager.h"
#include "Editor.h"
#include "Logger.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>

namespace NanoMark {

AutosaveManager::AutosaveManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &AutosaveManager::processAutosaves);
    m_timer->start(AUTOSAVE_INTERVAL_MS);
    
    QDir().mkpath(autosaveDirectory());
}

void AutosaveManager::watchEditor(Editor *editor, const QString &filePath)
{
    if (!editor) return;
    m_watchedEditors[editor] = filePath;
}

void AutosaveManager::unwatchEditor(Editor *editor)
{
    // Clean up recovery file if it exists and we're done
    if (m_watchedEditors.contains(editor)) {
        QString recPath = recoveryPath(m_watchedEditors[editor]);
        QFile::remove(recPath);
        m_watchedEditors.remove(editor);
    }
}

void AutosaveManager::processAutosaves()
{
    for (auto it = m_watchedEditors.begin(); it != m_watchedEditors.end(); ++it) {
        Editor *editor = it.key();
        if (editor->document()->isModified()) {
            saveRecovery(editor, it.value());
        }
    }
}

void AutosaveManager::forceAutosave()
{
    processAutosaves();
}

QString AutosaveManager::autosaveDirectory() const
{
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return docPath + "/NanoMark/Autosave";
}

QString AutosaveManager::recoveryPath(const QString &originalPath)
{
    if (originalPath.isEmpty()) return QString();
    
    // Hash the path to create a unique but stable filename
    QByteArray hash = QCryptographicHash::hash(originalPath.toUtf8(), QCryptographicHash::Md5);
    QString filename = hash.toHex() + ".bak";
    
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    return docPath + "/NanoMark/Autosave/" + filename;
}

bool AutosaveManager::hasRecovery(const QString &originalPath)
{
    QString path = recoveryPath(originalPath);
    return !path.isEmpty() && QFile::exists(path);
}

void AutosaveManager::saveRecovery(Editor *editor, const QString &originalPath)
{
    QString path = recoveryPath(originalPath);
    if (path.isEmpty()) return;

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(editor->toPlainText().toUtf8());
        file.close();
        Logger::instance().info("Autosaved recovery for: " + originalPath);
    }
}

} // namespace NanoMark
