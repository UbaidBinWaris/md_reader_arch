/**
 * NanoMark — ServiceRegistry.cpp
 * Central service locator implementation
 */

#include "ServiceRegistry.h"
#include "Logger.h"

namespace NanoMark {

ServiceRegistry::ServiceRegistry()
    : QObject(nullptr)
{
}

ServiceRegistry& ServiceRegistry::instance()
{
    static ServiceRegistry s_instance;
    return s_instance;
}

void ServiceRegistry::registerService(const QString &name, QObject *service)
{
    m_services[name] = service;
    Logger::instance().info("ServiceRegistry: Registered service '" + name + "'");
    emit serviceRegistered(name);
}

void ServiceRegistry::unregisterService(const QString &name)
{
    m_services.remove(name);
    Logger::instance().info("ServiceRegistry: Unregistered service '" + name + "'");
    emit serviceUnregistered(name);
}

QObject* ServiceRegistry::getService(const QString &name) const
{
    return m_services.value(name, nullptr);
}

bool ServiceRegistry::hasService(const QString &name) const
{
    return m_services.contains(name);
}

QStringList ServiceRegistry::serviceNames() const
{
    return m_services.keys();
}

} // namespace NanoMark
