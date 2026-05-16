/**
 * NanoMark — Application.cpp
 * Main application controller implementation
 */

#include "Application.h"
#include "MainWindow.h"
#include "ThemeManager.h"
#include "SettingsService.h"
#include "ErrorHandler.h"
#include "Logger.h"

#include <QFile>
#include <QFileInfo>

namespace NanoMark {

struct Application::Impl {
    std::unique_ptr<MainWindow> mainWindow;
};

Application::Application(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
}

Application::~Application() = default;

void Application::initialize(const QStringList &args)
{
    Logger::instance().info("NanoMark starting...");

    // Install crash handlers
    ErrorHandler::instance().install();

    // Initialize SQLite persistence
    SettingsService::instance().initialize();

    // Load saved theme or default to Dark
    int savedTheme = SettingsService::instance().get("theme", 0).toInt();
    ThemeManager::instance().loadTheme(static_cast<ThemeManager::Theme>(savedTheme));

    // Create main window
    m_impl->mainWindow = std::make_unique<MainWindow>();
    m_impl->mainWindow->show();

    // Open files passed via command line
    for (int i = 1; i < args.size(); ++i) {
        const QString &arg = args[i];
        QFileInfo fi(arg);
        if (fi.exists() && fi.isFile()) {
            m_impl->mainWindow->openFile(arg);
        }
    }

    Logger::instance().info("NanoMark initialized successfully.");
}

} // namespace NanoMark
