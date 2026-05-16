/**
 * NanoMark — WindowManager.cpp
 * Multi-window management implementation
 */

#include "WindowManager.h"
#include "MainWindow.h"
#include "Logger.h"

#include <QApplication>

namespace NanoMark {

WindowManager::WindowManager()
    : QObject(nullptr)
{
}

WindowManager& WindowManager::instance()
{
    static WindowManager s_instance;
    return s_instance;
}

MainWindow* WindowManager::createWindow()
{
    auto *window = new MainWindow();

    connect(window, &QObject::destroyed, this, &WindowManager::onWindowDestroyed);

    m_windows.append(window);
    window->show();

    Logger::instance().info("WindowManager: Created window #" + QString::number(m_windows.size()));
    emit windowCreated(window);

    return window;
}

QList<MainWindow*> WindowManager::windows() const
{
    return m_windows;
}

MainWindow* WindowManager::primaryWindow() const
{
    return m_windows.isEmpty() ? nullptr : m_windows.first();
}

void WindowManager::closeAll()
{
    // Close in reverse order
    for (int i = m_windows.size() - 1; i >= 0; --i) {
        m_windows[i]->close();
    }
}

int WindowManager::windowCount() const
{
    return m_windows.size();
}

void WindowManager::onWindowDestroyed(QObject *obj)
{
    auto *window = static_cast<MainWindow*>(obj);
    m_windows.removeOne(window);

    Logger::instance().info("WindowManager: Window closed. Remaining: " +
                           QString::number(m_windows.size()));
    emit windowClosed(window);

    if (m_windows.isEmpty()) {
        emit allWindowsClosed();
    }
}

} // namespace NanoMark
