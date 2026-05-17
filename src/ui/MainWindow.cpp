/**
 * NanoMark — MainWindow.cpp
 * Central application window implementation
 */

#include "MainWindow.h"
#include "Editor.h"
#include "PreviewPane.h"
#include "MarkdownRenderer.h"
#include "FileManager.h"
#include "WorkspaceManager.h"
#include "StudyMode.h"
#include "ThemeManager.h"
#include "SettingsService.h"
#include "AutosaveManager.h"
#include "Dashboard.h"
#include "CommandPalette.h"
#include "TabManager.h"
#include "Logger.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QTimer>
#include <QShortcut>
#include <QKeySequence>
#include <QFileSystemModel>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QTextBlock>
#include <QApplication>
#include <QStyle>

namespace NanoMark {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_renderer(std::make_unique<MarkdownRenderer>())
    , m_fileManager(std::make_unique<FileManager>(this))
    , m_workspaceManager(std::make_unique<WorkspaceManager>(this))
    , m_studyMode(std::make_unique<StudyMode>(this))
    , m_autosaveManager(std::make_unique<AutosaveManager>(this))
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupSidebar();
    setupShortcuts();
    restoreWindowState();

    m_renderTimer = new QTimer(this);
    m_renderTimer->setSingleShot(true);
    connect(m_renderTimer, &QTimer::timeout, this, &MainWindow::updatePreview);


    setAcceptDrops(true);
    setMinimumSize(900, 600);

    Logger::instance().info("MainWindow created.");
}

MainWindow::~MainWindow()
{
    saveWindowState();
}

// ─── UI Setup ──────────────────────────────────────────────────────────────────
void MainWindow::setupUI()
{
    setWindowTitle("NanoMark");
    resize(1400, 850);

    m_stack = new QStackedWidget(this);

    // Page 0: Dashboard
    m_dashboard = new Dashboard(this);
    connect(m_dashboard, &Dashboard::newFileRequested, this, &MainWindow::onNewFile);
    connect(m_dashboard, &Dashboard::openFileRequested, this, [this](const QString &path) {
        if (path.isEmpty()) onOpenFile();
        else openFile(path);
    });
    connect(m_dashboard, &Dashboard::openFolderRequested, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Workspace Folder"));
        if (!dir.isEmpty()) m_workspaceManager->openWorkspace(dir);
    });
    m_stack->addWidget(m_dashboard);

    // Page 1: Workspace Splitter
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setObjectName("MainSplitter");
    
    m_tabWidget = new QTabWidget(this);
    m_tabManager = std::make_unique<TabManager>(m_tabWidget, this);

    connect(m_tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);

    m_previewPane = new PreviewPane(this);

    m_splitter->addWidget(m_tabWidget);
    m_splitter->addWidget(m_previewPane);
    m_splitter->setSizes({650, 650});
    m_splitter->setHandleWidth(2);
    
    m_stack->addWidget(m_splitter);

    setCentralWidget(m_stack);
    updateWorkspaceVisibility();

    // Initialize Command Palette overlay
    m_commandPalette = new CommandPalette(this);
    connect(m_commandPalette, &CommandPalette::itemSelected, this, [this](const QVariant &data) {
        QString action = data.toString();
        if (action.startsWith("cmd:")) {
            QString cmd = action.mid(4);
            if (cmd == "new") onNewFile();
            else if (cmd == "open") onOpenFile();
            else if (cmd == "save") onSaveFile();
            else if (cmd == "save_as") onSaveFileAs();
            else if (cmd == "study") onToggleStudyMode();
            else if (cmd == "theme") onToggleTheme();
            else if (cmd == "pdf") onExportPDF();
            else if (cmd == "html") onExportHTML();
            else if (cmd == "sidebar") { if (m_sidebarDock) m_sidebarDock->setVisible(!m_sidebarDock->isVisible()); }
            else if (cmd == "preview") {
                m_previewPane->setVisible(!m_previewPane->isVisible());
                schedulePreviewUpdate();
            }
            else if (cmd == "quit") close();
        } else if (action.startsWith("file:")) {
            QString path = action.mid(5);
            openFile(path);
        }
    });
}

void MainWindow::updateWorkspaceVisibility()
{
    if (m_tabWidget->count() == 0) {
        m_stack->setCurrentWidget(m_dashboard);
        if (m_sidebarDock) m_sidebarDock->hide();
    } else {
        m_stack->setCurrentWidget(m_splitter);
        if (m_sidebarDock) m_sidebarDock->show();
    }
}

void MainWindow::setupMenuBar()
{
    // ── File Menu ──
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAction = fileMenu->addAction(tr("&New"), this, &MainWindow::onNewFile);
    newAction->setShortcut(QKeySequence::New);
    newAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));

    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::onOpenFile);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));

    fileMenu->addSeparator();

    QAction *saveAction = fileMenu->addAction(tr("&Save"), this, &MainWindow::onSaveFile);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

    QAction *saveAsAction = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onSaveFileAs);
    saveAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

    fileMenu->addSeparator();

    QMenu *exportMenu = fileMenu->addMenu(tr("&Export"));
    exportMenu->addAction(tr("Export as &PDF..."), this, &MainWindow::onExportPDF);
    exportMenu->addAction(tr("Export as &HTML..."), this, &MainWindow::onExportHTML);

    fileMenu->addSeparator();

    QAction *quitAction = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAction->setShortcut(QKeySequence::Quit);

    // ── Edit Menu ──
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    QAction *undoAction = editMenu->addAction(tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, [this]() {
        if (auto *e = currentEditor()) e->undo();
    });

    QAction *redoAction = editMenu->addAction(tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, [this]() {
        if (auto *e = currentEditor()) e->redo();
    });

    editMenu->addSeparator();

    QAction *cutAction = editMenu->addAction(tr("Cu&t"));
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, [this]() {
        if (auto *e = currentEditor()) e->cut();
    });

    QAction *copyAction = editMenu->addAction(tr("&Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, [this]() {
        if (auto *e = currentEditor()) e->copy();
    });

    QAction *pasteAction = editMenu->addAction(tr("&Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, [this]() {
        if (auto *e = currentEditor()) e->paste();
    });

    editMenu->addSeparator();

    QAction *findAction = editMenu->addAction(tr("&Find && Replace..."), this, &MainWindow::onFindReplace);
    findAction->setShortcut(QKeySequence::Find);

    // ── View Menu ──
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    QAction *toggleStudyAction = viewMenu->addAction(tr("Toggle &Study Mode"), this, &MainWindow::onToggleStudyMode);
    toggleStudyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    toggleStudyAction->setCheckable(true);

    QAction *toggleThemeAction = viewMenu->addAction(tr("Toggle &Theme"), this, &MainWindow::onToggleTheme);
    toggleThemeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));

    viewMenu->addSeparator();

    QAction *toggleSidebarAction = viewMenu->addAction(tr("Toggle Si&debar"));
    toggleSidebarAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    connect(toggleSidebarAction, &QAction::triggered, this, [this]() {
        if (m_sidebarDock) m_sidebarDock->setVisible(!m_sidebarDock->isVisible());
    });

    QAction *togglePreviewAction = viewMenu->addAction(tr("Toggle &Preview"));
    togglePreviewAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V));
    connect(togglePreviewAction, &QAction::triggered, this, [this]() {
        m_previewPane->setVisible(!m_previewPane->isVisible());
        schedulePreviewUpdate();
    });

    viewMenu->addSeparator();

    QAction *quickOpenAction = viewMenu->addAction(tr("Quick &Open..."), this, &MainWindow::onShowQuickOpen);
    quickOpenAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));

    QAction *cmdPaletteAction = viewMenu->addAction(tr("&Command Palette..."), this, &MainWindow::onShowCommandPalette);
    cmdPaletteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));

    // ── Help Menu ──
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About NanoMark"), this, &MainWindow::onShowAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolbar = addToolBar(tr("Markdown"));
    toolbar->setObjectName("MainToolbar");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(18, 18));

    // Text formatting buttons
    auto addFormatAction = [&](const QString &label, const QString &tooltip, auto slot) {
        QAction *action = toolbar->addAction(label);
        action->setToolTip(tooltip);
        connect(action, &QAction::triggered, this, slot);
    };

    addFormatAction("H1", "Heading 1 (Ctrl+1)", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("# ", "");
    });
    addFormatAction("H2", "Heading 2 (Ctrl+2)", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("## ", "");
    });
    addFormatAction("H3", "Heading 3 (Ctrl+3)", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("### ", "");
    });

    toolbar->addSeparator();

    addFormatAction("B", "Bold (Ctrl+B)", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("**", "**");
    });
    addFormatAction("I", "Italic (Ctrl+I)", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("*", "*");
    });
    addFormatAction("S", "Strikethrough", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("~~", "~~");
    });
    addFormatAction("Code", "Inline Code (Ctrl+`)", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("`", "`");
    });

    toolbar->addSeparator();

    addFormatAction("• List", "Unordered List", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("- ", "");
    });
    addFormatAction("1. List", "Ordered List", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("1. ", "");
    });
    addFormatAction("☐ Task", "Task List", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("- [ ] ", "");
    });

    toolbar->addSeparator();

    addFormatAction("Link", "Insert Link", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("[", "](url)");
    });
    addFormatAction("Image", "Insert Image", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("![alt](", ")");
    });
    addFormatAction("Table", "Insert Table", [this]() {
        if (auto *e = currentEditor()) e->insertAtCursor(
            "| Column 1 | Column 2 | Column 3 |\n"
            "|----------|----------|----------|\n"
            "| Cell 1   | Cell 2   | Cell 3   |\n");
    });
    addFormatAction("```", "Code Block", [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("```\n", "\n```");
    });

    toolbar->addSeparator();

    addFormatAction("> Quote", "Blockquote", [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("> ", "");
    });
    addFormatAction("— Rule", "Horizontal Rule", [this]() {
        if (auto *e = currentEditor()) e->insertAtCursor("\n---\n");
    });
}

void MainWindow::setupStatusBar()
{
    m_lineColLabel = new QLabel("Ln 1, Col 1");
    m_wordCountLabel = new QLabel("Words: 0");
    m_readTimeLabel = new QLabel("0 min read");
    m_fileSizeLabel = new QLabel("0 KB");
    m_encodingLabel = new QLabel("UTF-8");
    m_fileTypeLabel = new QLabel("Markdown");
    m_modeLabel = new QLabel("Developer Mode");

    statusBar()->addPermanentWidget(m_modeLabel);
    statusBar()->addPermanentWidget(m_fileTypeLabel);
    statusBar()->addPermanentWidget(m_encodingLabel);
    statusBar()->addPermanentWidget(m_fileSizeLabel);
    statusBar()->addPermanentWidget(m_readTimeLabel);
    statusBar()->addPermanentWidget(m_wordCountLabel);
    statusBar()->addPermanentWidget(m_lineColLabel);
}

void MainWindow::setupSidebar()
{
    // File Explorer sidebar
    m_sidebarDock = new QDockWidget(tr("Explorer"), this);
    m_sidebarDock->setObjectName("SidebarDock");
    m_sidebarDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_sidebarDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);

    m_fileTreeView = new QTreeView(m_sidebarDock);
    m_fileTreeView->setHeaderHidden(true);
    m_fileTreeView->setAnimated(true);
    m_fileTreeView->setIndentation(16);

    // Set up the file system model
    auto *fsModel = new QFileSystemModel(this);
    fsModel->setRootPath(QDir::homePath());
    fsModel->setNameFilters({"*.md", "*.markdown", "*.txt", "*.html"});
    fsModel->setNameFilterDisables(false);
    m_fileTreeView->setModel(fsModel);

    // Only show the name column
    for (int i = 1; i < fsModel->columnCount(); ++i)
        m_fileTreeView->hideColumn(i);

    connect(m_fileTreeView, &QTreeView::doubleClicked,
            this, &MainWindow::onSidebarFileClicked);

    m_sidebarDock->setWidget(m_fileTreeView);
    addDockWidget(Qt::LeftDockWidgetArea, m_sidebarDock);

    // Document Outline sidebar
    m_outlineDock = new QDockWidget(tr("Outline"), this);
    m_outlineDock->setObjectName("OutlineDock");
    m_outlineDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_outlineDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);

    QWidget *outlineContainer = new QWidget(m_outlineDock);
    QVBoxLayout *outlineLayout = new QVBoxLayout(outlineContainer);
    outlineLayout->setContentsMargins(4, 4, 4, 4);
    outlineLayout->setSpacing(4);

    m_outlineSearchEdit = new QLineEdit(outlineContainer);
    m_outlineSearchEdit->setPlaceholderText(tr("Filter headings..."));
    m_outlineSearchEdit->setStyleSheet(
        "QLineEdit {"
        "  background: #161616;"
        "  color: #f8f9fa;"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 4px;"
        "  padding: 4px 8px;"
        "  font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "  border-color: #007acc;"
        "}"
    );
    outlineLayout->addWidget(m_outlineSearchEdit);

    m_outlineView = new QTreeView(outlineContainer);
    m_outlineView->setHeaderHidden(true);
    m_outlineView->setFrameShape(QFrame::NoFrame);

    m_outlineModel = new QStandardItemModel(this);
    m_outlineFilterProxyModel = new QSortFilterProxyModel(this);
    m_outlineFilterProxyModel->setSourceModel(m_outlineModel);
    m_outlineFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_outlineFilterProxyModel->setFilterKeyColumn(0);

    m_outlineView->setModel(m_outlineFilterProxyModel);
    outlineLayout->addWidget(m_outlineView);

    m_outlineDock->setWidget(outlineContainer);
    connect(m_outlineView, &QTreeView::clicked, this, &MainWindow::onOutlineHeadingClicked);

    connect(m_outlineSearchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_outlineFilterProxyModel->setFilterFixedString(text);
    });

    addDockWidget(Qt::LeftDockWidgetArea, m_outlineDock);

    // Tab the two docks together
    tabifyDockWidget(m_sidebarDock, m_outlineDock);
    m_sidebarDock->raise();
}

void MainWindow::setupShortcuts()
{
    // Heading shortcuts
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_1), this, [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("# ", "");
    });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_2), this, [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("## ", "");
    });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_3), this, [this]() {
        if (auto *e = currentEditor()) e->insertFormatting("### ", "");
    });

    // Bold / Italic
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_B), this, [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("**", "**");
    });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_I), this, [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("*", "*");
    });
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_QuoteLeft), this, [this]() {
        if (auto *e = currentEditor()) e->wrapSelection("`", "`");
    });
}

// ─── Tab Management ────────────────────────────────────────────────────────────

void MainWindow::createNewTab(const QString &title, const QString &content)
{
    auto *editor = new Editor(this);
    if (!content.isEmpty()) {
        editor->setPlainText(content);
    }
    connect(editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::onEditorTextChanged);
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::schedulePreviewUpdate);
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::syncOutlineHighlight);
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, [this, editor]() {
        if (editor == currentEditor()) {
            if (m_previewPane->isVisible()) {
                QScrollBar *vBar = editor->verticalScrollBar();
                double percent = 0.0;
                if (vBar->maximum() > 0) {
                    percent = static_cast<double>(vBar->value()) / vBar->maximum();
                }
                m_previewPane->scrollToPercentage(percent);
            }
            syncOutlineHighlight(); // Track active heading position dynamically
        }
    });

    int idx = m_tabWidget->addTab(editor, title);
    m_tabWidget->setCurrentIndex(idx);
    
    // Watch editor for autosave (empty path = "Untitled")
    m_autosaveManager->watchEditor(editor, "");

    updateWorkspaceVisibility();
    schedulePreviewUpdate();
}

Editor* MainWindow::currentEditor() const
{
    return qobject_cast<Editor*>(m_tabWidget->currentWidget());
}

// ─── Slots ─────────────────────────────────────────────────────────────────────

void MainWindow::onNewFile()
{
    createNewTab();
}

void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Markdown File"), QString(),
        tr("Markdown Files (*.md *.markdown);;Text Files (*.txt);;All Files (*)"));

    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

void MainWindow::openFile(const QString &filePath)
{
    auto result = m_fileManager->readFile(filePath);
    if (!result.success) {
        QMessageBox::warning(this, tr("Error"),
            tr("Could not open file: %1\n%2").arg(filePath, result.errorMessage));
        return;
    }

    QFileInfo fi(filePath);
    createNewTab(fi.fileName(), result.content);

    // Store the file path in the editor
    if (auto *e = currentEditor()) {
        e->setProperty("filePath", filePath);
        m_autosaveManager->watchEditor(e, filePath);
    }

    m_fileManager->addRecentFile(filePath);
    updateWindowTitle();

    // Set sidebar to the file's directory
    if (auto *fsModel = qobject_cast<QFileSystemModel*>(m_fileTreeView->model())) {
        m_fileTreeView->setRootIndex(fsModel->index(fi.absolutePath()));
    }

    Logger::instance().info("Opened file: " + filePath);
    schedulePreviewUpdate();
}

void MainWindow::onSaveFile()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QString filePath = editor->property("filePath").toString();
    if (filePath.isEmpty()) {
        onSaveFileAs();
        return;
    }

    auto result = m_fileManager->writeFile(filePath, editor->toPlainText());
    if (!result.success) {
        QMessageBox::warning(this, tr("Error"),
            tr("Could not save file: %1").arg(result.errorMessage));
        return;
    }

    editor->document()->setModified(false);
    updateWindowTitle();
    statusBar()->showMessage(tr("Saved: %1").arg(filePath), 3000);
}

void MainWindow::onSaveFileAs()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Save Markdown File"), QString(),
        tr("Markdown Files (*.md);;All Files (*)"));

    if (filePath.isEmpty()) return;

    editor->setProperty("filePath", filePath);
    m_tabWidget->setTabText(m_tabWidget->currentIndex(), QFileInfo(filePath).fileName());

    onSaveFile();
}

void MainWindow::onExportPDF()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Export as PDF"), QString(), tr("PDF Files (*.pdf)"));

    if (filePath.isEmpty()) return;

    QString html = m_renderer->render(editor->toPlainText());
    m_previewPane->exportToPDF(filePath, html);
    statusBar()->showMessage(tr("Exported to PDF: %1").arg(filePath), 3000);
}

void MainWindow::onExportHTML()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Export as HTML"), QString(), tr("HTML Files (*.html)"));

    if (filePath.isEmpty()) return;

    QString html = m_renderer->renderStandalone(editor->toPlainText());
    auto result = m_fileManager->writeFile(filePath, html);
    if (result.success) {
        statusBar()->showMessage(tr("Exported to HTML: %1").arg(filePath), 3000);
    }
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index);
    updatePreview();
    updateStatusBar();
    updateWindowTitle();
}

void MainWindow::onTabCloseRequested(int index)
{
    auto *editor = qobject_cast<Editor*>(m_tabWidget->widget(index));
    if (!editor) return;

    if (editor->document()->isModified()) {
        auto ret = QMessageBox::question(this, tr("Unsaved Changes"),
            tr("The document has unsaved changes. Save before closing?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            m_tabWidget->setCurrentIndex(index);
            onSaveFile();
        } else if (ret == QMessageBox::Cancel) {
            return;
        }
    }

    m_autosaveManager->unwatchEditor(editor);
    m_tabWidget->removeTab(index);
    delete editor;

    updateWorkspaceVisibility();
}

void MainWindow::onEditorTextChanged()
{
    // Mark tab as modified
    int idx = m_tabWidget->currentIndex();
    QString title = m_tabWidget->tabText(idx);
    if (!title.endsWith(" •")) {
        m_tabWidget->setTabText(idx, title + " •");
    }

    if (auto *e = currentEditor()) {
        m_autosaveManager->markDirty(e);
    }

    schedulePreviewUpdate();
    updateStatusBar();
}

void MainWindow::onToggleStudyMode()
{
    m_isStudyMode = !m_isStudyMode;

    if (m_isStudyMode) {
        // Study mode: hide editor tabs, show full-width preview
        m_tabWidget->hide();
        m_previewPane->show();
        m_modeLabel->setText("Study Mode");

        // Make the current editor read-only
        if (auto *e = currentEditor()) {
            e->setReadOnly(true);
        }
    } else {
        // Developer mode: show split view
        m_tabWidget->show();
        m_previewPane->show();
        m_modeLabel->setText("Developer Mode");

        if (auto *e = currentEditor()) {
            e->setReadOnly(false);
        }
    }

    emit modeChanged(m_isStudyMode);
    schedulePreviewUpdate();
}

void MainWindow::onToggleTheme()
{
    auto &tm = ThemeManager::instance();
    if (tm.currentTheme() == ThemeManager::Theme::Dark) {
        tm.loadTheme(ThemeManager::Theme::Light);
    } else {
        tm.loadTheme(ThemeManager::Theme::Dark);
    }
    updatePreview();
}

void MainWindow::onShowAbout()
{
    QMessageBox::about(this, tr("About NanoMark"),
        tr("<h2>NanoMark v1.0.0</h2>"
           "<p>A modern, lightweight markdown editor and study workspace.</p>"
           "<p>Built with C++20 and Qt 6.</p>"
           "<p>&copy; 2026 NanoMark</p>"));
}

void MainWindow::onShowCommandPalette()
{
    QList<CommandPalette::ActionItem> items = {
        {"New File", "Create a blank markdown document", "cmd:new"},
        {"Open File...", "Browse for a file", "cmd:open"},
        {"Save", "Save the current document", "cmd:save"},
        {"Save As...", "Save document as a new file", "cmd:save_as"},
        {"Toggle Study Mode", "Switch to distraction-free reading", "cmd:study"},
        {"Toggle Theme", "Switch between Dark and Light themes", "cmd:theme"},
        {"Toggle Sidebar", "Show or hide the file explorer", "cmd:sidebar"},
        {"Toggle Preview", "Show or hide the markdown preview", "cmd:preview"},
        {"Export as PDF", "Save current document as PDF", "cmd:pdf"},
        {"Export as HTML", "Save current document as HTML", "cmd:html"},
        {"Quit", "Exit NanoMark", "cmd:quit"}
    };

    m_commandPalette->showOverlay(CommandPalette::CommandMode, items);
}

void MainWindow::onShowQuickOpen()
{
    QList<CommandPalette::ActionItem> items;
    
    // Add recent files
    auto recent = SettingsService::instance().recentFiles();
    for (const QString &path : recent) {
        QFileInfo fi(path);
        items.append({"📄 " + fi.fileName(), path, "file:" + path});
    }

    // Could add workspace files here by iterating the workspace directory

    m_commandPalette->showOverlay(CommandPalette::QuickOpenMode, items);
}

void MainWindow::onFindReplace()
{
    if (auto *e = currentEditor()) {
        e->showFindReplaceBar();
    }
}

void MainWindow::onSidebarFileClicked(const QModelIndex &index)
{
    auto *fsModel = qobject_cast<QFileSystemModel*>(m_fileTreeView->model());
    if (!fsModel) return;

    QString filePath = fsModel->filePath(index);
    QFileInfo fi(filePath);
    if (fi.isFile()) {
        openFile(filePath);
    }
}

void MainWindow::onOutlineHeadingClicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = m_outlineFilterProxyModel->mapToSource(index);
    QVariant data = sourceIndex.data(Qt::UserRole);
    if (!data.isValid()) return;

    int lineNum = data.toInt();
    auto *editor = currentEditor();
    if (!editor) return;

    QTextDocument *doc = editor->document();
    if (!doc) return;

    QTextBlock block = doc->findBlockByLineNumber(lineNum);
    if (block.isValid()) {
        QTextCursor cursor(block);
        editor->setTextCursor(cursor);
        
        // Scroll the viewport to the bottom first so that the target block is guaranteed to be above the viewport.
        editor->verticalScrollBar()->setValue(editor->verticalScrollBar()->maximum());
        // Force Qt to scroll up to make the cursor visible, which aligns it perfectly at the top of the viewport!
        editor->ensureCursorVisible();
        
        // Ensure the clicked outline tree item itself is perfectly scrolled and centered in view
        m_outlineView->scrollTo(index);
        m_outlineView->setCurrentIndex(index);
        
        editor->setFocus();
    }
}

// ─── Updates ───────────────────────────────────────────────────────────────────

void MainWindow::schedulePreviewUpdate()
{
    if (!m_previewPane->isVisible()) {
        m_renderTimer->start(20); // Still need it for status bar and outline updates
        return;
    }
    m_renderTimer->start(20); // 20ms debounce is completely imperceptible to the human eye, feeling absolutely instant and real-time!
}

void MainWindow::updatePreview()
{
    // Always update status bar on the debounced tick
    updateStatusBar();

    auto *editor = currentEditor();
    if (!editor) return;

    QString markdown = editor->toPlainText();

    // Always update outline (on keystrokes, loads, and tab switches) regardless of preview pane visibility!
    auto headings = m_renderer->extractHeadings(markdown);
    auto *model = m_outlineModel;
    if (model) {
        model->clear();
        if (headings.isEmpty()) {
            auto *item = new QStandardItem(tr("No headings found"));
            item->setEditable(false);
            QFont italicFont = item->font();
            italicFont.setItalic(true);
            item->setFont(italicFont);
            item->setForeground(QBrush(QColor("#777777")));
            model->appendRow(item);
        } else {
            for (const auto &h : headings) {
                auto *item = new QStandardItem(QString("  ").repeated(h.level - 1) + h.text);
                item->setEditable(false);
                item->setData(h.lineNumber, Qt::UserRole);
                model->appendRow(item);
            }
        }
    }

    syncOutlineHighlight(); // Update the highlight on the freshly populated outline model

    // Return early for WebEngine updates if preview pane is hidden
    if (!m_previewPane->isVisible()) return;

    bool isDark = ThemeManager::instance().currentTheme() == ThemeManager::Theme::Dark;
    QString css = m_renderer->getPreviewCss();

    if (editor->isHtmlDirty()) {
        QString htmlBody = m_renderer->renderBody(markdown);
        editor->setCachedHtml(htmlBody);
        m_previewPane->updatePreview(htmlBody, isDark, css);
    } else {
        // Cache hit! Instant rendering skip
        m_previewPane->updatePreview(editor->cachedHtml(), isDark, css);
    }
}

void MainWindow::updateStatusBar()
{
    auto *editor = currentEditor();
    if (!editor) return;

    // Line/Col (hide in Study Mode for cleaner reading)
    if (m_isStudyMode) {
        m_lineColLabel->hide();
    } else {
        m_lineColLabel->show();
        QTextCursor cursor = editor->textCursor();
        int line = cursor.blockNumber() + 1;
        int col = cursor.columnNumber() + 1;
        m_lineColLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));
    }

    // Word Count & Read Time
    QString text = editor->toPlainText();
    int wordCount = text.isEmpty() ? 0 : text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    m_wordCountLabel->setText(QString("Words: %1").arg(wordCount));

    int readTime = qMax(1, (wordCount + 199) / 200);
    m_readTimeLabel->setText(QString("%1 min read").arg(readTime));

    // File Size
    QString filePath = editor->property("filePath").toString();
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
        qint64 bytes = QFileInfo(filePath).size();
        if (bytes < 1024) {
            m_fileSizeLabel->setText(QString("%1 B").arg(bytes));
        } else if (bytes < 1024 * 1024) {
            m_fileSizeLabel->setText(QString("%1 KB").arg(qRound(bytes / 1024.0)));
        } else {
            m_fileSizeLabel->setText(QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1));
        }
    } else {
        m_fileSizeLabel->setText("0 KB");
    }

    m_modeLabel->setText(m_isStudyMode ? "Study Mode" : "Edit Mode");
}

void MainWindow::syncOutlineHighlight()
{
    auto *editor = currentEditor();
    if (!editor || !m_outlineModel || !m_outlineFilterProxyModel) return;

    // Use first visible block line number for natural reading position tracking
    int currentLine = editor->firstVisibleBlock().blockNumber() + 1;
    
    // Retrieve extracted headings from the renderer directly
    QString markdown = editor->toPlainText();
    auto headings = m_renderer->extractHeadings(markdown);
    if (headings.isEmpty()) return;

    int activeIndex = -1;
    for (int i = 0; i < headings.size(); ++i) {
        if (headings[i].lineNumber <= currentLine) {
            activeIndex = i;
        } else {
            break;
        }
    }

    if (activeIndex != -1) {
        QModelIndex srcIndex = m_outlineModel->index(activeIndex, 0);
        QModelIndex proxyIndex = m_outlineFilterProxyModel->mapFromSource(srcIndex);
        if (proxyIndex.isValid()) {
            m_outlineView->blockSignals(true);
            m_outlineView->setCurrentIndex(proxyIndex);
            m_outlineView->scrollTo(proxyIndex, QAbstractItemView::EnsureVisible);
            m_outlineView->blockSignals(false);
        }
    }
}

void MainWindow::updateWindowTitle()
{
    auto *editor = currentEditor();
    if (!editor) {
        setWindowTitle("NanoMark");
        return;
    }

    QString filePath = editor->property("filePath").toString();
    if (filePath.isEmpty()) {
        setWindowTitle("Untitled — NanoMark");
    } else {
        QString modified = editor->document()->isModified() ? " •" : "";
        setWindowTitle(QFileInfo(filePath).fileName() + modified + " — NanoMark");
    }
}

void MainWindow::saveWindowState()
{
    QList<SessionTab> tabs;
    for (int i = 0; i < m_tabManager->count(); ++i) {
        auto *editor = m_tabManager->editorAt(i);
        if (!editor) continue;

        SessionTab tab;
        tab.filePath = editor->property("filePath").toString();
        
        // Only save if it has a path
        if (tab.filePath.isEmpty()) continue;

        tab.cursorLine = editor->textCursor().blockNumber();
        tab.cursorCol = editor->textCursor().columnNumber();
        tab.scrollPosition = editor->verticalScrollBar()->value();
        tab.isActive = (i == m_tabWidget->currentIndex());
        tabs.append(tab);
    }

    auto &ss = SettingsService::instance();
    ss.saveSession(tabs, saveGeometry(), saveState(), m_splitter->saveState());
    ss.setLastWorkspace(m_workspaceManager->currentWorkspace());
    ss.set("isStudyMode", m_isStudyMode);
}

void MainWindow::restoreWindowState()
{
    auto &ss = SettingsService::instance();
    
    // Restore window geometry and state
    restoreGeometry(ss.restoreWindowGeometry());
    restoreState(ss.restoreWindowState());
    m_splitter->restoreState(ss.restoreSplitterState());

    // Validate splitter sizes to prevent 0-width panel collapses
    QList<int> splitterSizes = m_splitter->sizes();
    if (splitterSizes.size() < 2 || splitterSizes[0] < 50 || splitterSizes[1] < 50) {
        m_splitter->setSizes({600, 600});
    }

    // Restore workspace
    QString lastWs = ss.lastWorkspace();
    if (!lastWs.isEmpty()) {
        m_workspaceManager->openWorkspace(lastWs);
    }

    // Restore study mode
    m_isStudyMode = ss.get("isStudyMode", false).toBool();

    // Immediately display the window structure and dashboard (under 50ms startup!)
    updateWorkspaceVisibility();

    // Defer the heavy session and tab loading asynchronously to let event loop start instantly
    QTimer::singleShot(0, this, &MainWindow::restoreSessionAsync);
}

void MainWindow::restoreSessionAsync()
{
    auto &ss = SettingsService::instance();
    auto tabs = ss.restoreSessionTabs();
    
    // Check for recovery files
    QStringList recoveries = m_autosaveManager->getAvailableRecoveries();
    if (!recoveries.isEmpty()) {
        try {
            // ALWAYS restore from recovery files automatically without prompt dialogs!
            for (const QString &recPath : recoveries) {
                // Read recovery file
                QFile file(recPath);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString content = QString::fromUtf8(file.readAll());
                    
                    // Parse metadata
                    QString originalPath = "Untitled";
                    int metaEnd = content.indexOf("---AUTOSAVE-END-META---\n");
                    if (metaEnd != -1) {
                        QString meta = content.left(metaEnd);
                        if (meta.startsWith("OriginalPath: ")) {
                            originalPath = meta.mid(14).trimmed();
                        }
                        content = content.mid(metaEnd + 24); // skip tag and newline
                    }
                    
                    // Recreate tab
                    QString title = originalPath == "Untitled" ? "Untitled" : QFileInfo(originalPath).fileName();
                    createNewTab(title, content);
                    
                    if (auto *editor = currentEditor()) {
                        if (originalPath != "Untitled") {
                            editor->setProperty("filePath", originalPath);
                            m_autosaveManager->watchEditor(editor, originalPath);
                        }
                        // Force modified state since it's recovered
                        editor->document()->setModified(true);
                        m_autosaveManager->markDirty(editor);
                    }
                }
            }
        } catch (...) {
            Logger::instance().error("Failed to restore recovery files, starting clean session...");
        }
        
        // Clean up the old recoveries once handled
        m_autosaveManager->discardAllRecoveries();
        updateWorkspaceVisibility();
        schedulePreviewUpdate();
        return;
    }

    if (tabs.isEmpty()) {
        updateWorkspaceVisibility();
    } else {
        // Remove the default empty tab if it exists
        if (m_tabWidget->count() > 0) {
            auto *w = m_tabWidget->widget(0);
            m_tabWidget->removeTab(0);
            delete w;
        }

        int activeIdx = -1;
        for (int i = 0; i < tabs.size(); ++i) {
            const auto &tab = tabs[i];
            if (QFile::exists(tab.filePath)) {
                openFile(tab.filePath);
                
                // Set positions
                if (auto *editor = currentEditor()) {
                    QTextCursor cursor = editor->textCursor();
                    editor->verticalScrollBar()->setValue(tab.scrollPosition);
                }

                if (tab.isActive) {
                    activeIdx = m_tabWidget->count() - 1;
                }
            }
        }

        if (activeIdx != -1) {
            m_tabWidget->setCurrentIndex(activeIdx);
        }
        updateWorkspaceVisibility();
        schedulePreviewUpdate();
    }
}

// ─── Events ────────────────────────────────────────────────────────────────────

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Check all tabs for unsaved changes using authoritative tab manager
    for (int i = 0; i < m_tabManager->count(); ++i) {
        auto *editor = m_tabManager->editorAt(i);
        if (editor && editor->document()->isModified()) {
            m_tabWidget->setCurrentIndex(i);
            auto ret = QMessageBox::question(this, tr("Unsaved Changes"),
                tr("You have unsaved changes. Quit anyway?"),
                QMessageBox::Yes | QMessageBox::No);

            if (ret == QMessageBox::No) {
                event->ignore();
                return;
            }
            break;
        }
    }

    saveWindowState();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls) {
        if (url.isLocalFile()) {
            openFile(url.toLocalFile());
        }
    }
}

} // namespace NanoMark
