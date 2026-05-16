/**
 * NanoMark — SettingsService.cpp
 * SQLite-backed settings and session persistence
 */

#include "SettingsService.h"
#include "Logger.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>

namespace NanoMark {

SettingsService::SettingsService() : QObject(nullptr) {}
SettingsService::~SettingsService() = default;

SettingsService& SettingsService::instance()
{
    static SettingsService s_instance;
    return s_instance;
}

QString SettingsService::databasePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return dataDir + "/nanomark.db";
}

bool SettingsService::initialize()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "nanomark_settings");
    db.setDatabaseName(databasePath());

    if (!db.open()) {
        Logger::instance().error("SettingsService: Failed to open database: " + db.lastError().text());
        return false;
    }

    createTables();
    Logger::instance().info("SettingsService: Database initialized at " + databasePath());
    return true;
}

void SettingsService::createTables()
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);

    q.exec("CREATE TABLE IF NOT EXISTS settings ("
           "  key TEXT PRIMARY KEY,"
           "  value TEXT"
           ")");

    q.exec("CREATE TABLE IF NOT EXISTS recent_files ("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  path TEXT UNIQUE,"
           "  opened_at DATETIME DEFAULT CURRENT_TIMESTAMP"
           ")");

    q.exec("CREATE TABLE IF NOT EXISTS session_tabs ("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  file_path TEXT,"
           "  cursor_line INTEGER DEFAULT 0,"
           "  cursor_col INTEGER DEFAULT 0,"
           "  scroll_pos INTEGER DEFAULT 0,"
           "  is_active INTEGER DEFAULT 0,"
           "  tab_order INTEGER DEFAULT 0"
           ")");

    q.exec("CREATE TABLE IF NOT EXISTS session_state ("
           "  key TEXT PRIMARY KEY,"
           "  value BLOB"
           ")");
}

// ─── Generic Settings ──────────────────────────────────────────────────────

QVariant SettingsService::get(const QString &key, const QVariant &defaultValue) const
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.prepare("SELECT value FROM settings WHERE key = ?");
    q.addBindValue(key);
    if (q.exec() && q.next()) {
        return q.value(0);
    }
    return defaultValue;
}

void SettingsService::set(const QString &key, const QVariant &value)
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.prepare("INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)");
    q.addBindValue(key);
    q.addBindValue(value.toString());
    q.exec();
    emit settingChanged(key, value);
}

// ─── Recent Files ──────────────────────────────────────────────────────────

QStringList SettingsService::recentFiles() const
{
    QStringList files;
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.exec("SELECT path FROM recent_files ORDER BY opened_at DESC LIMIT " +
           QString::number(MAX_RECENT_FILES));
    while (q.next()) {
        files.append(q.value(0).toString());
    }
    return files;
}

void SettingsService::addRecentFile(const QString &filePath)
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);

    // Remove if exists (to update timestamp)
    q.prepare("DELETE FROM recent_files WHERE path = ?");
    q.addBindValue(filePath);
    q.exec();

    // Insert fresh
    q.prepare("INSERT INTO recent_files (path) VALUES (?)");
    q.addBindValue(filePath);
    q.exec();

    // Trim old entries
    q.exec("DELETE FROM recent_files WHERE id NOT IN "
           "(SELECT id FROM recent_files ORDER BY opened_at DESC LIMIT " +
           QString::number(MAX_RECENT_FILES) + ")");
}

void SettingsService::clearRecentFiles()
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.exec("DELETE FROM recent_files");
}

// ─── Session Management ────────────────────────────────────────────────────

void SettingsService::saveSession(const QList<SessionTab> &tabs,
                                   const QByteArray &windowGeometry,
                                   const QByteArray &windowState,
                                   const QByteArray &splitterState)
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);

    // Clear old session
    q.exec("DELETE FROM session_tabs");
    q.exec("DELETE FROM session_state");

    // Save tabs
    q.prepare("INSERT INTO session_tabs (file_path, cursor_line, cursor_col, scroll_pos, is_active, tab_order) "
              "VALUES (?, ?, ?, ?, ?, ?)");

    for (int i = 0; i < tabs.size(); ++i) {
        const auto &tab = tabs[i];
        q.addBindValue(tab.filePath);
        q.addBindValue(tab.cursorLine);
        q.addBindValue(tab.cursorCol);
        q.addBindValue(tab.scrollPosition);
        q.addBindValue(tab.isActive ? 1 : 0);
        q.addBindValue(i);
        q.exec();
    }

    // Save window state
    auto saveBlob = [&](const QString &key, const QByteArray &data) {
        QSqlQuery sq(db);
        sq.prepare("INSERT INTO session_state (key, value) VALUES (?, ?)");
        sq.addBindValue(key);
        sq.addBindValue(data);
        sq.exec();
    };

    saveBlob("window_geometry", windowGeometry);
    saveBlob("window_state", windowState);
    saveBlob("splitter_state", splitterState);

    Logger::instance().info("Session saved: " + QString::number(tabs.size()) + " tabs");
}

QList<SessionTab> SettingsService::restoreSessionTabs() const
{
    QList<SessionTab> tabs;
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.exec("SELECT file_path, cursor_line, cursor_col, scroll_pos, is_active FROM session_tabs ORDER BY tab_order");

    while (q.next()) {
        SessionTab tab;
        tab.filePath = q.value(0).toString();
        tab.cursorLine = q.value(1).toInt();
        tab.cursorCol = q.value(2).toInt();
        tab.scrollPosition = q.value(3).toInt();
        tab.isActive = q.value(4).toBool();
        tabs.append(tab);
    }
    return tabs;
}

QByteArray SettingsService::restoreWindowGeometry() const
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.prepare("SELECT value FROM session_state WHERE key = 'window_geometry'");
    if (q.exec() && q.next()) return q.value(0).toByteArray();
    return {};
}

QByteArray SettingsService::restoreWindowState() const
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.prepare("SELECT value FROM session_state WHERE key = 'window_state'");
    if (q.exec() && q.next()) return q.value(0).toByteArray();
    return {};
}

QByteArray SettingsService::restoreSplitterState() const
{
    QSqlDatabase db = QSqlDatabase::database("nanomark_settings");
    QSqlQuery q(db);
    q.prepare("SELECT value FROM session_state WHERE key = 'splitter_state'");
    if (q.exec() && q.next()) return q.value(0).toByteArray();
    return {};
}

void SettingsService::setLastWorkspace(const QString &path)
{
    set("last_workspace", path);
}

QString SettingsService::lastWorkspace() const
{
    return get("last_workspace").toString();
}

} // namespace NanoMark
