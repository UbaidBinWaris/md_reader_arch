/**
 * NanoMark — ErrorHandler.cpp
 * Global error handling implementation
 */

#include "ErrorHandler.h"
#include "Logger.h"

#include <QMessageBox>
#include <QApplication>
#include <csignal>
#include <iostream>

namespace NanoMark {

// Signal handler for crashes
static void crashHandler(int signal)
{
    std::cerr << "NanoMark: Caught signal " << signal << std::endl;
    Logger::instance().error(QString("CRASH: Signal %1 caught").arg(signal));

    // Attempt to save any unsaved work
    // (In a full implementation, this would iterate open editors and auto-save)

    std::exit(signal);
}

ErrorHandler::ErrorHandler()
    : QObject(nullptr)
{
}

ErrorHandler& ErrorHandler::instance()
{
    static ErrorHandler s_instance;
    return s_instance;
}

void ErrorHandler::install()
{
    // Install signal handlers for crash recovery
    std::signal(SIGSEGV, crashHandler);
    std::signal(SIGABRT, crashHandler);
    std::signal(SIGFPE, crashHandler);

    Logger::instance().info("Error handlers installed.");
}

void ErrorHandler::handleError(const QString &context, const QString &message)
{
    Logger::instance().error(context + ": " + message);
    emit errorOccurred(context, message);

    if (m_errorCallback) {
        m_errorCallback(context, message);
    }
}

void ErrorHandler::handleFatalError(const QString &context, const QString &message)
{
    Logger::instance().error("FATAL - " + context + ": " + message);

    QMessageBox::critical(nullptr, "Fatal Error",
        QString("A fatal error occurred:\n\n%1\n\n%2\n\nThe application will now close.")
            .arg(context, message));

    QApplication::exit(1);
}

void ErrorHandler::setErrorCallback(std::function<void(const QString&, const QString&)> callback)
{
    m_errorCallback = std::move(callback);
}

} // namespace NanoMark
