#pragma once

/**
 * NanoMark — ServiceRegistry.h
 * Central service locator for modular service-oriented architecture
 */

#include <QObject>
#include <QHash>
#include <QString>

namespace NanoMark {

class ServiceRegistry : public QObject
{
    Q_OBJECT

public:
    static ServiceRegistry& instance();

    /** Register a service by name */
    void registerService(const QString &name, QObject *service);

    /** Unregister a service */
    void unregisterService(const QString &name);

    /** Get a service by name */
    QObject* getService(const QString &name) const;

    /** Check if a service is registered */
    bool hasService(const QString &name) const;

    /** Get a typed service */
    template<typename T>
    T* service(const QString &name) const {
        return qobject_cast<T*>(getService(name));
    }

    /** List all registered services */
    QStringList serviceNames() const;

signals:
    void serviceRegistered(const QString &name);
    void serviceUnregistered(const QString &name);

private:
    ServiceRegistry();

    QHash<QString, QObject*> m_services;
};

} // namespace NanoMark
