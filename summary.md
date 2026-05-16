# NanoMark — Project Technical Summary

## Project Overview

NanoMark is a lightweight Markdown editing and study workspace application written in modern C++ (C++20) using Qt 6. It provides a split editor/preview UI, workspace file browsing, annotations, sticky notes, export to HTML/PDF, theming, and study modes designed for reading and revision.

This document summarizes the project structure, build and runtime setup, and provides a module-by-module technical reference for each class and primary function/behavior implemented in the codebase.

---

## Quick Build & Run

Prerequisites:
- Qt 6 (including Qt WebEngine)
- C++20-compatible compiler (gcc/clang/MSVC)
- CMake (>=3.16 recommended)

Typical build steps (from project root):

```bash
mkdir -p build
cd build
cmake ..
cmake --build . --parallel
# Run the built binary (example target `NanoMark`)
./NanoMark
```

If you use an IDE (Qt Creator, CLion), open the top-level `CMakeLists.txt` and configure a Qt6 kit.

---

## Project Layout

- `src/` — application source
  - `core/` — `Application` entrypoint
  - `ui/` — `MainWindow` and UI glue
  - `editor/` — `Editor`, `MarkdownHighlighter`
  - `markdown/` — `MarkdownRenderer`
  - `preview/` — `PreviewPane` (QWebEngineView wrapper)
  - `filemanager/` — `FileManager`, `WorkspaceManager`
  - `export/` — `ExportManager`, `HTMLExporter`, `PDFExporter`
  - `study/` — `Annotations`, `StickyNotes`, `StudyMode`
  - `themes/` — `ThemeManager`
  - `utils/` — `Logger`, `ErrorHandler`
- `resources/` — `resources.qrc`, themes and preview CSS

---

## High-level Architecture

- Single-process desktop app using Qt event loop and widgets.
- UI composed around `MainWindow` with a `QTabWidget` for editors and a `PreviewPane` on the right (split layout).
- Markdown rendering (for preview/export) is handled by `MarkdownRenderer` which converts a markdown string to an HTML fragment and a standalone HTML document.
- File I/O and recent-file persistence via `FileManager` (uses Qt `QFile` and `QSettings`).
- Workspace tree is provided by `WorkspaceManager` using `QFileSystemModel`.
- Export orchestration: `ExportManager` uses `MarkdownRenderer` and `FileManager`; PDF export is performed via `PreviewPane` (QWebEnginePage::printToPdf).
- Persistent per-document artifacts (stickies, annotations) are serialized to JSON arrays.
- Global services: `Logger` (singleton) and `ErrorHandler` (singleton-style instance) provide logging and crash handling.

---

## Module Reference (classes & functions)

Below are the main modules and the important classes/methods with concise descriptions derived from the implementation.

- `Application` (src/core)
  - `void initialize(const QStringList &args)` — sets up theme, creates and shows `MainWindow`, opens files passed on the command line.

- `MainWindow` (src/ui)
  - `MainWindow(QWidget*)` — constructs UI: menus, toolbars, status bar, sidebar, outline, a `QSplitter` containing editors and preview.
  - `openFile(const QString &filePath)` — reads file via `FileManager`, creates a new editor tab, sets editor property `filePath`, updates sidebar and recent files.
  - `onSaveFile()` / `onSaveFileAs()` — saves current tab content through `FileManager` using atomic save (writes temp file then renames).
  - `onExportPDF()` / `onExportHTML()` — export flows: HTML uses `MarkdownRenderer` + `FileManager`; PDF uses `PreviewPane::exportToPDF` (QWebEngine printing).
  - `createNewTab(title, content)` — sets up a new `Editor` and connects text and cursor signals.
  - UI helpers: `setupMenuBar()`, `setupToolBar()`, `setupSidebar()`, `setupStatusBar()`, `setupShortcuts()`.
  - Event handlers: drag-and-drop, tab close, unsaved-change prompts, theme toggle, study mode toggle (switches read-only & view behaviors).

- `Editor` (src/editor)
  - `insertFormatting(prefix, suffix)` — inserts or wraps text at the current line/selection.
  - `wrapSelection(before, after)` — wraps selected text or inserts markers and places cursor between them.
  - `insertAtCursor(text)` — inserts raw text at cursor.
  - `showFindReplaceBar()` — presents a small find/replace UI with Find, Replace, Replace All.
  - `lineNumberAreaWidth()`, `updateLineNumberAreaWidth()`, `lineNumberAreaPaintEvent()` — line numbering implementation and painter.
  - `keyPressEvent(QKeyEvent*)` — implements auto-indent, auto-list continuation on Enter, tab-to-spaces and multi-line indenting for selections, Escape closes find bar.
  - Uses `MarkdownHighlighter` attached to the document for syntax coloring.

- `MarkdownHighlighter` (src/editor)
  - `highlightBlock(const QString &text)` — applies regex-based rules for headings, bold, italic, lists, blockquotes, fenced code blocks (maintains state across blocks for code fences).
  - Rules are configured for common GFM constructs; code blocks have monospaced format and background.

- `MarkdownRenderer` (src/markdown)
  - `QString render(const QString &markdown) const` — produces a complete HTML document including CSS and highlight.js integration for preview.
  - `QString renderStandalone(const QString &markdown) const` — alias to `render` (used for export).
  - `QString markdownToHtml(const QString &markdown) const` — main conversion: handles headings, paragraphs, fenced code blocks (```), lists (ordered/unordered), task lists, blockquotes, horizontal rules, tables (pipe-style), and inline formatting.
  - `QString processInline(const QString &line) const` — converts inline elements: images, links, bold, italic, strikethrough, inline code. Uses escaping via `escapeHtml`.
  - `QVector<Heading> extractHeadings(const QString &markdown) const` — returns headings with level and line number for outline generation.
  - Important notes: conversion is implemented with QRegularExpression and is intended for reasonably-sized documents; it's not a full CommonMark implementation but supports common GFM features.

- `PreviewPane` (src/preview)
  - Inherits `QWebEngineView`. `setHtml(const QString &html)` updates preview while attempting to preserve scroll position (reads window.scrollY and restores it after setHtml).
  - `exportToPDF(const QString &filePath, const QString &html)` — creates a transient QWebEnginePage, sets HTML, waits for loadFinished, then calls `printToPdf` with an A4 layout and margins. The export page is scheduled for deletion after a delay.

- `FileManager` (src/filemanager)
  - `FileResult readFile(const QString &filePath)` — opens file in read-only UTF-8, returns contents or an error (logs via `Logger`).
  - `FileResult writeFile(const QString &filePath, const QString &content)` — atomic save: write to `file.tmp`, close, remove original, rename temp to destination. Returns error messages on failure.
  - `QStringList recentFiles()` / `addRecentFile()` / `clearRecentFiles()` — uses `QSettings` for persistence; keeps a `MAX_RECENT_FILES` limit.

- `WorkspaceManager` (src/filemanager)
  - Uses `QFileSystemModel` to expose a filesystem tree filtered for markdown and related files. `openWorkspace(dirPath)` sets model root and emits `workspaceChanged`.

- `ExportManager` / `HTMLExporter` / `PDFExporter` (src/export)
  - `ExportManager::exportToHTML` — renders a standalone HTML using `MarkdownRenderer` and writes it to disk; emits `exportCompleted` or `exportFailed` signals.
  - `ExportManager::exportToPDF` — currently a convenience/delegation; PDF export is handled through `PreviewPane`.
  - `HTMLExporter::exportToHTML` — helper that performs render+write similar to `ExportManager`.
  - `PDFExporter::exportToPDF` — placeholder (logs request) — main PDF flow uses `QWebEnginePage::printToPdf` in `PreviewPane`.

- `StudyMode` (src/study)
  - Provides toggles for `active`, `zoomLevel`, and `focusMode`. Emits signals on changes. Zoom is clamped between 50 and 300%.

- `StickyNotes` and `Annotations` (src/study)
  - `StickyNote` and `Annotation` structs include `toJson()` and `fromJson()` helpers.
  - `addNote` / `removeNote` / `updateNote` / `notesForDocument` — notes are stored in-memory in a `QVector` and can be serialized to JSON via `save(filePath)` and restored via `load(filePath)`.
  - `Annotations` supports `bookmarks()` and `annotationsForDocument()`.

- `ThemeManager` (src/themes)
  - Singleton-style `instance()`.
  - `loadTheme(Theme t)` — loads the QSS from resources (`:/themes/dark.qss` or `:/themes/light.qss`) and applies it via `qApp->setStyleSheet`. Also persists choice to `QSettings`.
  - `previewCssPath()` — returns resource path to preview CSS for renderer.

- `ErrorHandler` (src/utils)
  - Singleton-like `instance()` and `install()` function that registers signal handlers (SIGSEGV, SIGABRT, SIGFPE) to attempt graceful logging and exit. `handleError` and `handleFatalError` log and optionally show a message box; supports a user-provided callback.

- `Logger` (src/utils)
  - Thread-safe-ish logging via `QMutex` and `QMutexLocker`. `instance()` gives singleton access. Writes to `stderr` and optionally to a log file if `setLogFile()` is used. Supports levels: Debug, Info, Warning, Error.

---

## Data & File Formats

- Recent files: stored in `QSettings` under `recentFiles` (QStringList).
- Sticky notes and annotations: persisted as JSON arrays. Each element is a JSON object matching `StickyNote::toJson()` / `Annotation::toJson()` fields (id, document path, line numbers, content, color, timestamps).
- Exports: HTML is a standalone HTML file produced by `MarkdownRenderer::renderStandalone`. PDF export uses the previewer's `printToPdf` output.

---

## Error Handling & Logging

- The app uses `ErrorHandler::install()` to register POSIX signals; `Logger` is used throughout to record errors and informational events.
- `FileManager` reports file open/write failures with human-readable messages (taken from `QFile::errorString()`), and `MainWindow` surfaces them to the user via `QMessageBox`.

---

## Developer Notes & Extensibility

- Markdown parsing is implemented in `MarkdownRenderer` using regex-based rules; for full CommonMark compliance consider integrating a dedicated parser (e.g., cmark-gfm) and adapting the renderer to the chosen HTML/CSS skins.
- `PDFExporter` is a lightweight placeholder — advanced PDF options (margins, header/footer, pagination control) should be implemented either via `QPrinter`/`QTextDocument` or by enhancing the `QWebEnginePage::printToPdf` call with callbacks.
- Consider adding unit tests for `MarkdownRenderer` and `FileManager` using a test framework (e.g., Catch2, QtTest) to protect parsing and I/O behavior.

---

## How to Contribute

- Follow the existing code style (Qt APIs, QObject signaling, singletons for Logger/ThemeManager).
- Add small, focused commits with descriptive messages. Update `resources.qrc` for new assets and ensure QSS and preview CSS are present for consistent theming.

---

## File Added

This summary is saved as `summary.md` at the repository root.

---

If you want, I can:
- open a PR with this file,
- run a local build (if you want me to run CMake here), or
- generate a more exhaustive per-line API reference in Markdown or Doxygen format.
