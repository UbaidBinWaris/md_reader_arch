#pragma once

/**
 * NanoMark — AutosaveManager.h
 * Background autosave and recovery system
 */

#include <QObject>
#include <QString>
#include <QTimer>
#include <QHash>
#include <QUuid>

namespace NanoMark {

class Editor;

struct AutosaveInfo {
    QString uuid;
    QString originalPath;
    QString autosavePath;
    bool isDirty;
};

class AutosaveManager : public QObject
{
    Q_OBJECT

public:
    explicit AutosaveManager(QObject *parent = nullptr);

    /** Start watching an editor for changes. Generates a UUID for new "Untitled" files. */
    void watchEditor(Editor *editor, const QString &filePath = "");

    /** Stop watching an editor and optionally clean up its recovery file */
    void unwatchEditor(Editor *editor, bool cleanUp = true);

    /** Force an immediate autosave for all watched editors */
    void forceAutosave();

    /** Get a list of all existing recovery files (paths to the .autosave files) */
    QStringList getAvailableRecoveries() const;

    /** Clean up all recovery files (e.g. if user discards) */
    void discardAllRecoveries();

public slots:
    void markDirty(Editor *editor);

private slots:
    void processAutosaves();

private:
    QString autosaveDirectory() const;
    void saveRecovery(Editor *editor, AutosaveInfo &info);

    QTimer *m_timer = nullptr;
    QHash<Editor*, AutosaveInfo> m_watchedEditors;
    static constexpr int AUTOSAVE_INTERVAL_MS = 15000; // 15 seconds
};

} // namespace NanoMark
