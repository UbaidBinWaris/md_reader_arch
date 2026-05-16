#pragma once

/**
 * NanoMark — AutosaveManager.h
 * Background autosave and recovery system
 */

#include <QObject>
#include <QString>
#include <QTimer>
#include <QHash>

namespace NanoMark {

class Editor;

class AutosaveManager : public QObject
{
    Q_OBJECT

public:
    explicit AutosaveManager(QObject *parent = nullptr);

    /** Start watching an editor for changes */
    void watchEditor(Editor *editor, const QString &filePath);

    /** Stop watching an editor */
    void unwatchEditor(Editor *editor);

    /** Force an immediate autosave for all watched editors */
    void forceAutosave();

    /** Get the recovery path for a given file */
    static QString recoveryPath(const QString &originalPath);

    /** Check if a recovery file exists */
    static bool hasRecovery(const QString &originalPath);

private slots:
    void processAutosaves();

private:
    QString autosaveDirectory() const;
    void saveRecovery(Editor *editor, const QString &originalPath);

    QTimer *m_timer = nullptr;
    QHash<Editor*, QString> m_watchedEditors;
    static constexpr int AUTOSAVE_INTERVAL_MS = 30000; // 30 seconds
};

} // namespace NanoMark
