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
#include "Logger.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
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
#include <QApplication>
#include <QStyle>

namespace NanoMark {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_renderer(std::make_unique<MarkdownRenderer>())
    , m_fileManager(std::make_unique<FileManager>(this))
    , m_workspaceManager(std::make_unique<WorkspaceManager>(this))
    , m_studyMode(std::make_unique<StudyMode>(this))
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupSidebar();
    setupShortcuts();
    restoreWindowState();

    // Start with one empty tab
    createNewTab();

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

    // Central splitter: tabs (editor) on left, preview on right
    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);

    connect(m_tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);

    m_previewPane = new PreviewPane(this);

    m_splitter->addWidget(m_tabWidget);
    m_splitter->addWidget(m_previewPane);
    m_splitter->setSizes({650, 650});
    m_splitter->setHandleWidth(2);

    setCentralWidget(m_splitter);
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
    togglePreviewAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(togglePreviewAction, &QAction::triggered, this, [this]() {
        m_previewPane->setVisible(!m_previewPane->isVisible());
    });

    QAction *cmdPaletteAction = viewMenu->addAction(tr("&Command Palette..."), this, &MainWindow::onShowCommandPalette);
    cmdPaletteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));

    // ── Help Menu ──
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About NanoMark"), this, &MainWindow::onShowAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolbar = addToolBar(tr("Markdown"));
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
    m_encodingLabel = new QLabel("UTF-8");
    m_fileTypeLabel = new QLabel("Markdown");
    m_modeLabel = new QLabel("Developer Mode");

    statusBar()->addPermanentWidget(m_modeLabel);
    statusBar()->addPermanentWidget(m_fileTypeLabel);
    statusBar()->addPermanentWidget(m_encodingLabel);
    statusBar()->addPermanentWidget(m_wordCountLabel);
    statusBar()->addPermanentWidget(m_lineColLabel);
}

void MainWindow::setupSidebar()
{
    // File Explorer sidebar
    m_sidebarDock = new QDockWidget(tr("Explorer"), this);
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
    m_outlineDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_outlineDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);

    m_outlineView = new QTreeView(m_outlineDock);
    m_outlineView->setHeaderHidden(true);
    m_outlineView->setModel(new QStandardItemModel(this));
    m_outlineDock->setWidget(m_outlineView);
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
            this, &MainWindow::updateStatusBar);

    int idx = m_tabWidget->addTab(editor, title);
    m_tabWidget->setCurrentIndex(idx);
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
    }

    m_fileManager->addRecentFile(filePath);
    updateWindowTitle();

    // Set sidebar to the file's directory
    if (auto *fsModel = qobject_cast<QFileSystemModel*>(m_fileTreeView->model())) {
        m_fileTreeView->setRootIndex(fsModel->index(fi.absolutePath()));
    }

    Logger::instance().info("Opened file: " + filePath);
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

    m_tabWidget->removeTab(index);
    delete editor;

    // If no tabs remain, create a new one
    if (m_tabWidget->count() == 0) {
        createNewTab();
    }
}

void MainWindow::onEditorTextChanged()
{
    updatePreview();
    updateStatusBar();

    // Mark tab as modified
    int idx = m_tabWidget->currentIndex();
    QString title = m_tabWidget->tabText(idx);
    if (!title.endsWith(" •")) {
        m_tabWidget->setTabText(idx, title + " •");
    }
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
    bool ok;
    QString command = QInputDialog::getText(this, tr("Command Palette"),
        tr("Enter command:"), QLineEdit::Normal, "", &ok);

    if (!ok || command.isEmpty()) return;

    // Basic command handling
    command = command.toLower().trimmed();
    if (command == "new" || command == "new file") onNewFile();
    else if (command == "open") onOpenFile();
    else if (command == "save") onSaveFile();
    else if (command == "save as") onSaveFileAs();
    else if (command == "study" || command == "study mode") onToggleStudyMode();
    else if (command == "theme" || command == "toggle theme") onToggleTheme();
    else if (command == "pdf" || command == "export pdf") onExportPDF();
    else if (command == "html" || command == "export html") onExportHTML();
    else if (command == "quit" || command == "exit") close();
    else {
        statusBar()->showMessage(tr("Unknown command: %1").arg(command), 3000);
    }
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

// ─── Updates ───────────────────────────────────────────────────────────────────

void MainWindow::updatePreview()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QString markdown = editor->toPlainText();
    QString html = m_renderer->render(markdown);
    m_previewPane->setHtml(html);

    // Update outline
    auto headings = m_renderer->extractHeadings(markdown);
    auto *model = qobject_cast<QStandardItemModel*>(m_outlineView->model());
    if (model) {
        model->clear();
        for (const auto &h : headings) {
            auto *item = new QStandardItem(QString("  ").repeated(h.level - 1) + h.text);
            item->setEditable(false);
            model->appendRow(item);
        }
    }
}

void MainWindow::updateStatusBar()
{
    auto *editor = currentEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    m_lineColLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));

    QString text = editor->toPlainText();
    int wordCount = text.isEmpty() ? 0 : text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    m_wordCountLabel->setText(QString("Words: %1").arg(wordCount));
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
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
}

void MainWindow::restoreWindowState()
{
    QSettings settings;
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());
}

// ─── Events ────────────────────────────────────────────────────────────────────

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Check all tabs for unsaved changes
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        auto *editor = qobject_cast<Editor*>(m_tabWidget->widget(i));
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
