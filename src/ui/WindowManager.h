#pragma once

/**
 * NanoMark — WindowManager.h
 * Manages main window + secondary windows (Open in New Window)
 */

#include <QObject>
#include <QList>
#include <memory>

namespace NanoMark {

class MainWindow;

class WindowManager : public QObject
{
    Q_OBJECT

public:
    static WindowManager& instance();

    /** Create and show a new main window */
    MainWindow* createWindow();

    /** Get all active windows */
    QList<MainWindow*> windows() const;

    /** Get the primary window */
    MainWindow* primaryWindow() const;

    /** Close all windows */
    void closeAll();

    /** Window count */
    int windowCount() const;

signals:
    void windowCreated(MainWindow *window);
    void windowClosed(MainWindow *window);
    void allWindowsClosed();

private:
    WindowManager();

    void onWindowDestroyed(QObject *obj);

    QList<MainWindow*> m_windows;
};

} // namespace NanoMark
