#pragma once

/**
 * NanoMark — MainWindow.h
 * Central application window with split editor/preview layout
 */

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QLabel>
#include <QDockWidget>
#include <QTreeView>
#include <QTimer>
#include <QStackedWidget>
#include <memory>

namespace NanoMark {

class Editor;
class PreviewPane;
class MarkdownRenderer;
class FileManager;
class WorkspaceManager;
class StudyMode;
class ThemeManager;
class AutosaveManager;
class Dashboard;
class CommandPalette;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /** Open a file by path */
    void openFile(const QString &filePath);

    /** Get current editor */
    Editor* currentEditor() const;

signals:
    void modeChanged(bool studyMode);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onExportPDF();
    void onExportHTML();
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onEditorTextChanged();
    void onToggleStudyMode();
    void onToggleTheme();
    void onShowAbout();
    void onShowCommandPalette();
    void onShowQuickOpen();
    void onFindReplace();
    void onSidebarFileClicked(const QModelIndex &index);
    void restoreSessionAsync();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupSidebar();
    void setupShortcuts();
    void createNewTab(const QString &title = "Untitled", const QString &content = "");
    void schedulePreviewUpdate();
    void updatePreview();
    void updateStatusBar();
    void updateWindowTitle();
    void saveWindowState();
    void restoreWindowState();
    void updateWorkspaceVisibility();

    // UI Components
    QStackedWidget *m_stack = nullptr;
    QSplitter *m_splitter = nullptr;
    QTabWidget *m_tabWidget = nullptr;
    Dashboard *m_dashboard = nullptr;
    PreviewPane *m_previewPane = nullptr;
    CommandPalette *m_commandPalette = nullptr;
    QDockWidget *m_sidebarDock = nullptr;
    QTreeView *m_fileTreeView = nullptr;
    QDockWidget *m_outlineDock = nullptr;
    QTreeView *m_outlineView = nullptr;

    // Status bar labels
    QLabel *m_lineColLabel = nullptr;
    QLabel *m_wordCountLabel = nullptr;
    QLabel *m_readTimeLabel = nullptr;
    QLabel *m_fileSizeLabel = nullptr;
    QLabel *m_encodingLabel = nullptr;
    QLabel *m_fileTypeLabel = nullptr;
    QLabel *m_modeLabel = nullptr;

    // Managers
    std::unique_ptr<MarkdownRenderer> m_renderer;
    std::unique_ptr<FileManager> m_fileManager;
    std::unique_ptr<WorkspaceManager> m_workspaceManager;
    std::unique_ptr<StudyMode> m_studyMode;
    std::unique_ptr<AutosaveManager> m_autosaveManager;

    QTimer *m_renderTimer = nullptr;
    bool m_isStudyMode = false;
};

} // namespace NanoMark
