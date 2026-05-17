# NanoMark — Technical Summary (Phase 3.2 Optimization Updated)

## Overview
NanoMark is a C++20/Qt6 desktop markdown editor and study workspace.
Binary: build/NanoMark (11 MB) | Build: cmake + make | Status: COMPILES & RUNS

## Tech Stack
- **Language**: C++20
- **Framework**: Qt 6.11 (Widgets, WebEngine, PrintSupport, Sql, Test)
- **Persistence**: **SQLite 3**
- **UI**: Premium OpenAI/ChatGPT-inspired Dark/Light Modern
- **Preview**: Chromium-based WebEngine + Inter Font

## Project Structure
```text
src/
  core/        main.cpp, Application.h/.cpp, SettingsService.h/.cpp, ServiceRegistry.h/.cpp, PluginInterface.h
  ui/          MainWindow.h/.cpp, TabManager.h/.cpp, WindowManager.h/.cpp, Dashboard.h/.cpp
  editor/      Editor.h/.cpp, MarkdownHighlighter.h/.cpp
  markdown/    MarkdownRenderer.h/.cpp
  preview/     PreviewPane.h/.cpp
  filemanager/ FileManager.h/.cpp, WorkspaceManager.h/.cpp, AutosaveManager.h/.cpp
  study/       StudyMode.h/.cpp, StickyNotes.h/.cpp, Annotations.h/.cpp, NanomarkFile.h/.cpp
  themes/      ThemeManager.h/.cpp
  export/      ExportManager.h/.cpp, PDFExporter.h/.cpp, HTMLExporter.h/.cpp
  utils/       Logger.h/.cpp, ErrorHandler.h/.cpp
resources/
  themes/      dark.qss, light.qss (Premium OpenAI Themes)
  resources.qrc
tests/         CMakeLists.txt, test_renderer.cpp
```

---

## Module 1: core — Application & Infrastructure

### Application.cpp & main.cpp
- Initializes **ErrorHandler**, **SettingsService**, and **ThemeManager**.
- Creates the primary **MainWindow** via **WindowManager**.
- **Phase 3.1**: Sets global window icon (`logo.svg`) and silences Linux Chromium Vulkan/GPU warnings via `qputenv`.

### SettingsService.h / .cpp
- **Engine**: SQLite 3 (`nanomark.db`).
- **Tables**: `settings`, `recent_files`, `session_tabs`, `session_state`.
- **Key Methods**: `saveSession()`, `restoreSessionTabs()`, `addRecentFile()`.
- **Phase 3.2 Optimizations**: Operates in **WAL mode** (`PRAGMA journal_mode=WAL;`) for non-blocking concurrent I/O. `saveSession()` leverages **Batch Transactions** (`QSqlDatabase::transaction()`), collapsing dozens of flushes into a single atomic write for lightning-fast application shutdowns.

### ServiceRegistry.h / .cpp
- **Pattern**: Central Service Locator.
- **Purpose**: Decouples modules; provides typed access to services (Editor, Preview, etc.).

### PluginInterface.h
- **IID**: `com.nanomark.PluginInterface/1.0`.
- Defines lifecycle: `initialize(ServiceRegistry*)` and `shutdown()`.

---

## Module 2: ui — Window, Tab & Dashboard Management

### WindowManager.h / .cpp
- **Singleton**: Manages multiple `MainWindow` instances.
- **Multi-Window**: Supports "Open in New Window" for independent workspace contexts.

### TabManager.h / .cpp
- **Logic Extraction**: Handles tab creation, closing, and reordering.
- **Context Menu**: "Close", "Close Others", "Close All".
- **State**: Tracks modified status with "•" indicators.

### Dashboard.h / .cpp (Redesigned in Phase 3.1)
- **Welcome Screen**: Beautiful welcome dashboard displayed when no files are open.
- **Design**: Premium VS Code / ChatGPT aesthetic adhering strictly to a 4px spacing grid.
- **Features**: Recent documents list, Quick Start actions, Pinned placeholders, and Tips/Shortcuts.

### CommandPalette.h / .cpp (New in Phase 3.2)
- **UI Architecture**: A custom, frameless overlay widget inspired by Raycast and VS Code.
- **Quick Open (`Ctrl+P`)**: Fuzzy-search over recent files and workspace directories.
- **Command Palette (`Ctrl+Shift+P`)**: Searchable index of core application actions (e.g., Export, Theme, Study Mode).
- **Navigation**: Full keyboard focus handling (`Up`, `Down`, `Enter`, `Escape`).

### MainWindow.cpp (Upgraded)
- Now acts as a coordinator between `TabManager`, `WorkspaceManager`, `PreviewPane`, and `Dashboard`.
- **Status Bar Upgrades**: Dynamic "Read Time" (200 wpm calculation) and "File Size" formatting (KB/MB).
- Contextually hides Line/Col information in Study Mode.
- **Phase 3.1 Fixes**: Explicitly sets `setObjectName()` on docks and splitters to guarantee robust layout persistence (`saveState()`/`restoreState()`). Includes a `QMessageBox` on startup to recover unsaved `.autosave` sessions.

---

## Module 3: editor — Premium Text Editing

### Editor.cpp
- **Theme**: Harmonized with deep black `#0d0d0d` background.
- **Line Numbers**: Dynamic coloring (highlighted current line).
- **Behavior**: Auto-indentation, list continuation, 4-space tabs.

### MarkdownHighlighter.cpp
- **Visuals**: One Dark Modern regex rules.
- **States**: Fenced code blocks detection for background shading.

---

## Module 4: markdown — Rendering Engine & Preview

### MarkdownRenderer.cpp (Upgraded)
- **CSS Engine**: Premium styling inspired by ChatGPT.
- **Typography**: Inter font stack, optimized line heights (1.75).
- **Accents**: OpenAI green (`#10a37f`) for links, code blocks, and blockquotes.
- **Tables**: Smooth hover states and rounded corners.

### Preview Performance (Phase 3.2 Optimization)
- **Deferred Initialization**: `PreviewPane` acts as a lightweight `QWidget` placeholder on cold boot. The heavy `QWebEngineView` (Chromium) is lazily instantiated *only* when the preview actually receives content, drastically cutting initial memory overhead and startup lag.
- **Debounced Rendering**: Preview updates use a 250ms debounce timer to keep UI smooth during rapid typing.
- **Lazy Rendering**: Updates are paused when the preview is hidden or when in Study Mode.

---

## Module 5: filemanager — Autosave & File I/O

### AutosaveManager.h / .cpp (Overhauled in Phase 3.1)
- **Background Autosave**: Periodically creates `.autosave` files using generated **UUIDs** (e.g., `session_<UUID>.autosave`) in `~/Documents/NanoMark/Autosave/`.
- **Tracking**: Correctly tracks and saves unsaved "Untitled" documents. Connects to `QPlainTextEdit::textChanged`.
- **Metadata**: Embeds original file paths directly inside the backup file for seamless restoration.
- **Frequency**: Every 15 seconds for actively modified documents.
- **Cleanup**: Removes backup files upon successful manual save or clean close.

---

## Module 6: study — Portable Annotations

### NanomarkFile.h / .cpp
- **Format**: `.nanomark` sidecar file (JSON).
- **Content**: Sticky notes, Annotations, Bookmarks, and Reading Progress.
- **Workflow**: Portable metadata stays next to `.md` without polluting original source.

### StudyMode.cpp
- **Mode**: Read-only focus mode.
- **View**: Continuous smooth HTML rendering.

---

## Module 7: themes — Premium Design System

### dark.qss / light.qss (Upgraded)
- **Design System**: 4px spacing grid.
- **Palette**: Deep blacks (`#0d0d0d`), subtle borders (`#2e2e2e`), and green accents.
- **Focus Rings**: Removed default Qt focus outlines (`*:focus { outline: none; }`).
- **Scrollbars**: Modern, thin semi-transparent scrollbars (6px) that smoothly expand on hover (10px).

---

## Module 8: tests — Quality Assurance (New in Phase 3)

### QtTest Integration
- Setup unit testing infrastructure using `Qt6::Test`.
- `test_renderer.cpp`: Verifies Markdown parsing and HTML output (headings, bold, tables).

---

## Build Instructions
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./NanoMark

# Run Tests
ctest --output-on-failure
```

## Binary Info
- **Size**: ~11 MB (Phase 3 Debug)
- **Dependencies**: Qt6 (Core, Gui, Widgets, WebEngineWidgets, PrintSupport, Sql, Test)
- **Persistence**: SQLite 3
