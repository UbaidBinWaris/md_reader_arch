#pragma once

/**
 * NanoMark — ErrorHandler.h
 * Global error handling and crash recovery
 */

#include <QObject>
#include <QString>
#include <functional>

namespace NanoMark {

class ErrorHandler : public QObject
{
    Q_OBJECT

public:
    static ErrorHandler& instance();

    /** Install global exception handlers */
    void install();

    /** Handle a recoverable error */
    void handleError(const QString &context, const QString &message);

    /** Handle a fatal error */
    void handleFatalError(const QString &context, const QString &message);

    /** Set a custom error callback */
    void setErrorCallback(std::function<void(const QString&, const QString&)> callback);

signals:
    void errorOccurred(const QString &context, const QString &message);

private:
    ErrorHandler();
    std::function<void(const QString&, const QString&)> m_errorCallback;
};

} // namespace NanoMark
