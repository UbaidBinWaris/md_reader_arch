/**
 * NanoMark — main.cpp
 * Application entry point
 */

#include "Application.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    // Silence harmless Vulkan/GPU warnings on Linux WebEngine
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu --disable-vulkan --disable-gpu-compositing");

    // Enable high-DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication qtApp(argc, argv);
    qtApp.setApplicationName("NanoMark");
    qtApp.setApplicationVersion("1.0.0");
    qtApp.setOrganizationName("NanoMark");
    qtApp.setOrganizationDomain("nanomark.dev");
    qtApp.setWindowIcon(QIcon(":/logo/logo.svg"));

    // Initialize the NanoMark application controller
    NanoMark::Application app;
    app.initialize(qtApp.arguments());

    return qtApp.exec();
}
