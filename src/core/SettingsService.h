#pragma once

/**
 * NanoMark — SettingsService.h
 * SQLite-backed settings, session, and workspace persistence
 */

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>

namespace NanoMark {

struct SessionTab {
    QString filePath;
    int cursorLine = 0;
    int cursorCol = 0;
    int scrollPosition = 0;
    bool isActive = false;
};

class SettingsService : public QObject
{
    Q_OBJECT

public:
    static SettingsService& instance();

    /** Initialize the database (creates if needed) */
    bool initialize();

    /** Generic settings get/set */
    QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void set(const QString &key, const QVariant &value);

    /** Recent files */
    QStringList recentFiles() const;
    void addRecentFile(const QString &filePath);
    void clearRecentFiles();

    /** Session management */
    void saveSession(const QList<SessionTab> &tabs, const QByteArray &windowGeometry,
                     const QByteArray &windowState, const QByteArray &splitterState);
    QList<SessionTab> restoreSessionTabs() const;
    QByteArray restoreWindowGeometry() const;
    QByteArray restoreWindowState() const;
    QByteArray restoreSplitterState() const;

    /** Workspace state */
    void setLastWorkspace(const QString &path);
    QString lastWorkspace() const;

signals:
    void settingChanged(const QString &key, const QVariant &value);

private:
    SettingsService();
    ~SettingsService();

    void createTables();
    QString databasePath() const;

    static constexpr int MAX_RECENT_FILES = 20;
};

} // namespace NanoMark
