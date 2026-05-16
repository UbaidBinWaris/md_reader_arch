/**
 * NanoMark — main.cpp
 * Application entry point
 */

#include "Application.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Enable high-DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication qtApp(argc, argv);
    qtApp.setApplicationName("NanoMark");
    qtApp.setApplicationVersion("1.0.0");
    qtApp.setOrganizationName("NanoMark");
    qtApp.setOrganizationDomain("nanomark.dev");

    // Initialize the NanoMark application controller
    NanoMark::Application app;
    app.initialize(qtApp.arguments());

    return qtApp.exec();
}
