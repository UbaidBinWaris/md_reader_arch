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

namespace NanoMark {

AutosaveManager::AutosaveManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_debounceTimer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &AutosaveManager::processAutosaves);
    m_timer->start(AUTOSAVE_INTERVAL_MS);
    
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer, &QTimer::timeout, this, &AutosaveManager::processAutosaves);
    
    QDir().mkpath(autosaveDirectory());
}

void AutosaveManager::watchEditor(Editor *editor, const QString &filePath)
{
    if (!editor) return;
    
    AutosaveInfo info;
    info.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    info.originalPath = filePath.isEmpty() ? "Untitled" : filePath;
    info.autosavePath = autosaveDirectory() + "/session_" + info.uuid + ".autosave";
    info.isDirty = false;

    m_watchedEditors[editor] = info;
}

void AutosaveManager::unwatchEditor(Editor *editor, bool cleanUp)
{
    if (m_watchedEditors.contains(editor)) {
        if (cleanUp) {
            QString recPath = m_watchedEditors[editor].autosavePath;
            if (QFile::exists(recPath)) {
                QFile::remove(recPath);
                Logger::instance().info("Cleaned up autosave: " + recPath);
            }
        }
        m_watchedEditors.remove(editor);
    }
}

void AutosaveManager::markDirty(Editor *editor)
{
    if (m_watchedEditors.contains(editor)) {
        if (editor->isReadOnly()) return;
        m_watchedEditors[editor].isDirty = true;
        m_debounceTimer->start(200); // 200ms debounce: triggers autosave instantly when you pause typing!
    }
}

void AutosaveManager::processAutosaves()
{
    for (auto it = m_watchedEditors.begin(); it != m_watchedEditors.end(); ++it) {
        Editor *editor = it.key();
        AutosaveInfo &info = it.value();
        
        if (editor->isReadOnly()) {
            continue;
        }
        
        if (info.isDirty || editor->document()->isModified()) {
            saveRecovery(editor, info);
            info.isDirty = false;
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

QStringList AutosaveManager::getAvailableRecoveries() const
{
    QStringList recoveries;
    QDir dir(autosaveDirectory());
    QStringList filters;
    filters << "session_*.autosave";
    dir.setNameFilters(filters);
    
    for (const QFileInfo &fi : dir.entryInfoList(QDir::Files)) {
        recoveries << fi.absoluteFilePath();
    }
    return recoveries;
}

void AutosaveManager::discardAllRecoveries()
{
    QStringList recoveries = getAvailableRecoveries();
    for (const QString &path : recoveries) {
        QFile::remove(path);
    }
}

void AutosaveManager::saveRecovery(Editor *editor, AutosaveInfo &info)
{
    if (info.autosavePath.isEmpty()) return;

    QFile file(info.autosavePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Write metadata header
        file.write("OriginalPath: " + info.originalPath.toUtf8() + "\n");
        file.write("---AUTOSAVE-END-META---\n");
        // Write content
        file.write(editor->toPlainText().toUtf8());
        file.close();
        Logger::instance().info("Autosaved recovery to: " + info.autosavePath);
    }
}

} // namespace NanoMark
