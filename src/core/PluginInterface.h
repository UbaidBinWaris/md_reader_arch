#pragma once

/**
 * NanoMark — PluginInterface.h
 * Abstract interface for NanoMark plugins (Qt Plugin System)
 */

#include <QString>
#include <QtPlugin>

namespace NanoMark {

class ServiceRegistry;

class PluginInterface
{
public:
    virtual ~PluginInterface() = default;

    /** Plugin identifier */
    virtual QString name() const = 0;

    /** Plugin version */
    virtual QString version() const = 0;

    /** Plugin description */
    virtual QString description() const = 0;

    /** Called when plugin is loaded */
    virtual bool initialize(ServiceRegistry *registry) = 0;

    /** Called when plugin is unloaded */
    virtual void shutdown() = 0;
};

} // namespace NanoMark

#define NanoMark_PluginInterface_iid "com.nanomark.PluginInterface/1.0"
Q_DECLARE_INTERFACE(NanoMark::PluginInterface, NanoMark_PluginInterface_iid)
