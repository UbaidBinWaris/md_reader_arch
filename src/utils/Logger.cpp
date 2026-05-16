/**
 * NanoMark — Logger.cpp
 * Application-wide logging implementation
 */

#include "Logger.h"

#include <QDateTime>
#include <QTextStream>
#include <QMutexLocker>
#include <iostream>

namespace NanoMark {

Logger::Logger() = default;

Logger::~Logger()
{
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }
}

Logger& Logger::instance()
{
    static Logger s_instance;
    return s_instance;
}

void Logger::setLogFile(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }

    m_logFile = new QFile(filePath);
    if (!m_logFile->open(QIODevice::Append | QIODevice::Text)) {
        std::cerr << "Logger: Failed to open log file: "
                  << filePath.toStdString() << std::endl;
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void Logger::debug(const QString &message)   { log(Level::Debug, message); }
void Logger::info(const QString &message)    { log(Level::Info, message); }
void Logger::warning(const QString &message) { log(Level::Warning, message); }
void Logger::error(const QString &message)   { log(Level::Error, message); }

void Logger::log(Level level, const QString &message)
{
    if (level < m_level) return;

    QMutexLocker locker(&m_mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString formatted = QString("[%1] [%2] %3")
        .arg(timestamp, levelString(level), message);

    // Output to stderr
    std::cerr << formatted.toStdString() << std::endl;

    // Output to log file if available
    if (m_logFile && m_logFile->isOpen()) {
        QTextStream stream(m_logFile);
        stream << formatted << "\n";
        stream.flush();
    }
}

QString Logger::levelString(Level level) const
{
    switch (level) {
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO ";
        case Level::Warning: return "WARN ";
        case Level::Error:   return "ERROR";
    }
    return "?????";
}

} // namespace NanoMark
