#pragma once

/**
 * NanoMark — TabManager.h
 * Extracted tab management with context menus, reordering, and state tracking
 */

#include <QObject>
#include <QTabWidget>
#include <QString>

namespace NanoMark {

class Editor;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget *tabWidget, QObject *parent = nullptr);

    /** Create a new tab with optional content */
    Editor* createTab(const QString &title = "Untitled", const QString &content = "");

    /** Close tab at index, returns false if user cancelled */
    bool closeTab(int index);

    /** Close all tabs except the one at index */
    void closeOtherTabs(int exceptIndex);

    /** Close all tabs */
    void closeAllTabs();

    /** Get current editor */
    Editor* currentEditor() const;

    /** Get editor at index */
    Editor* editorAt(int index) const;

    /** Get tab count */
    int count() const;

    /** Mark tab as modified */
    void markModified(int index);

    /** Mark tab as saved */
    void markSaved(int index);

signals:
    void tabCreated(int index);
    void tabClosed(int index);
    void currentTabChanged(int index);

private:
    void setupContextMenu();

    QTabWidget *m_tabWidget;
};

} // namespace NanoMark
