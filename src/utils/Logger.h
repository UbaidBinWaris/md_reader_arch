#pragma once

/**
 * NanoMark — Logger.h
 * Application-wide logging system
 */

#include <QString>
#include <QFile>
#include <QMutex>

namespace NanoMark {

class Logger
{
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    /** Get the singleton instance */
    static Logger& instance();

    /** Set minimum log level */
    void setLevel(Level level) { m_level = level; }

    /** Set log file path (optional, logs to stderr by default) */
    void setLogFile(const QString &filePath);

    /** Log messages */
    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

private:
    Logger();
    ~Logger();

    void log(Level level, const QString &message);
    QString levelString(Level level) const;

    Level m_level = Level::Info;
    QFile *m_logFile = nullptr;
    QMutex m_mutex;
};

} // namespace NanoMark
