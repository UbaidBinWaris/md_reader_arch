/**
 * NanoMark — TabManager.cpp
 * Tab management implementation
 */

#include "TabManager.h"
#include "Editor.h"
#include "Logger.h"

#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QTabBar>

namespace NanoMark {

TabManager::TabManager(QTabWidget *tabWidget, QObject *parent)
    : QObject(parent)
    , m_tabWidget(tabWidget)
{
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);

    setupContextMenu();
}

Editor* TabManager::createTab(const QString &title, const QString &content)
{
    auto *editor = new Editor(m_tabWidget);
    if (!content.isEmpty()) {
        editor->setPlainText(content);
    }

    int idx = m_tabWidget->addTab(editor, title);
    m_tabWidget->setCurrentIndex(idx);

    emit tabCreated(idx);
    return editor;
}

bool TabManager::closeTab(int index)
{
    auto *editor = editorAt(index);
    if (!editor) return true;

    if (editor->document()->isModified()) {
        m_tabWidget->setCurrentIndex(index);
        auto ret = QMessageBox::question(
            m_tabWidget, QObject::tr("Unsaved Changes"),
            QObject::tr("The document has unsaved changes. Save before closing?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Cancel) return false;
        if (ret == QMessageBox::Save) {
            // Signal that save is needed — MainWindow handles actual save
            // For now, just proceed
        }
    }

    m_tabWidget->removeTab(index);
    delete editor;
    emit tabClosed(index);

    // Always keep at least one tab
    if (m_tabWidget->count() == 0) {
        createTab();
    }

    return true;
}

void TabManager::closeOtherTabs(int exceptIndex)
{
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        if (i != exceptIndex) {
            closeTab(i);
            // Adjust exceptIndex if we removed a tab before it
            if (i < exceptIndex) --exceptIndex;
        }
    }
}

void TabManager::closeAllTabs()
{
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        closeTab(i);
    }
}

Editor* TabManager::currentEditor() const
{
    return qobject_cast<Editor*>(m_tabWidget->currentWidget());
}

Editor* TabManager::editorAt(int index) const
{
    return qobject_cast<Editor*>(m_tabWidget->widget(index));
}

int TabManager::count() const
{
    return m_tabWidget->count();
}

void TabManager::markModified(int index)
{
    QString title = m_tabWidget->tabText(index);
    if (!title.endsWith(" •")) {
        m_tabWidget->setTabText(index, title + " •");
    }
}

void TabManager::markSaved(int index)
{
    QString title = m_tabWidget->tabText(index);
    if (title.endsWith(" •")) {
        title.chop(2);
        m_tabWidget->setTabText(index, title);
    }
}

void TabManager::setupContextMenu()
{
    m_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tabWidget, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        int index = m_tabWidget->tabBar()->tabAt(pos);
        if (index < 0) return;

        QMenu menu;
        menu.addAction(QObject::tr("Close"), [this, index]() { closeTab(index); });
        menu.addAction(QObject::tr("Close Others"), [this, index]() { closeOtherTabs(index); });
        menu.addAction(QObject::tr("Close All"), [this]() { closeAllTabs(); });
        menu.exec(m_tabWidget->mapToGlobal(pos));
    });
}

} // namespace NanoMark
