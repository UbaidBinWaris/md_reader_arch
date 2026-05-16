# NanoMark — Technical Summary (Phase 2 Updated)

## Overview
NanoMark is a C++20/Qt6 desktop markdown editor and study workspace.
Binary: build/NanoMark (9.7MB) | Build: cmake + make | Status: COMPILES & RUNS

## Tech Stack
- **Language**: C++20
- **Framework**: Qt 6.11 (Widgets, WebEngine, PrintSupport, **Sql**)
- **Persistence**: **SQLite 3**
- **UI**: Premium OpenAI/ChatGPT-inspired Dark/Light Modern
- **Preview**: Chromium-based WebEngine + Inter Font

## Project Structure
```
src/
  core/        main.cpp, Application.h/.cpp, SettingsService.h/.cpp, ServiceRegistry.h/.cpp, PluginInterface.h
  ui/          MainWindow.h/.cpp, TabManager.h/.cpp, WindowManager.h/.cpp
  editor/      Editor.h/.cpp, MarkdownHighlighter.h/.cpp
  markdown/    MarkdownRenderer.h/.cpp
  preview/     PreviewPane.h/.cpp
  filemanager/ FileManager.h/.cpp, WorkspaceManager.h/.cpp
  study/       StudyMode.h/.cpp, StickyNotes.h/.cpp, Annotations.h/.cpp, NanomarkFile.h/.cpp
  themes/      ThemeManager.h/.cpp
  export/      ExportManager.h/.cpp, PDFExporter.h/.cpp, HTMLExporter.h/.cpp
  utils/       Logger.h/.cpp, ErrorHandler.h/.cpp
resources/
  themes/      dark.qss, light.qss (Premium OpenAI Themes)
  resources.qrc
```

---

## Module 1: core — Application & Infrastructure

### Application.cpp
- Initializes **ErrorHandler**, **SettingsService**, and **ThemeManager**.
- Creates the primary **MainWindow** via **WindowManager**.

### SettingsService.h / .cpp (New in Phase 2)
- **Engine**: SQLite 3 (`nanomark.db`).
- **Tables**:
    - `settings`: Key-value pairs.
    - `recent_files`: Tracked with timestamps.
    - `session_tabs`: Paths, cursor positions, scroll positions.
    - `session_state`: Binary blobs for window geometry/state.
- **Key Methods**: `saveSession()`, `restoreSessionTabs()`, `addRecentFile()`.

### ServiceRegistry.h / .cpp (New in Phase 2)
- **Pattern**: Central Service Locator.
- **Purpose**: Decouples modules; provides typed access to services (Editor, Preview, etc.).
- **Method**: `service<T>(name)` for type-safe retrieval.

### PluginInterface.h (New in Phase 2)
- **IID**: `com.nanomark.PluginInterface/1.0`.
- Defines lifecycle: `initialize(ServiceRegistry*)` and `shutdown()`.

---

## Module 2: ui — Window & Tab Management

### WindowManager.h / .cpp (New in Phase 2)
- **Singleton**: Manages multiple `MainWindow` instances.
- **Multi-Window**: Supports "Open in New Window" for independent workspace contexts.

### TabManager.h / .cpp (New in Phase 2)
- **Logic Extraction**: Handles tab creation, closing, and reordering.
- **Context Menu**: "Close", "Close Others", "Close All".
- **State**: Tracks modified status with "•" indicators.

### MainWindow.cpp (Upgraded)
- Now acts as a coordinator between `TabManager`, `WorkspaceManager`, and `PreviewPane`.
- Supports dockable panels (Explorer, Outline).

---

## Module 3: editor — Premium Text Editing

### Editor.cpp (Upgraded)
- **Theme**: Harmonized with deep black `#0d0d0d` background.
- **Line Numbers**: Dynamic coloring (highlighted current line).
- **Behavior**: Auto-indentation, list continuation, 4-space tabs.

### MarkdownHighlighter.cpp
- **Visuals**: One Dark Modern regex rules.
- **States**: Fenced code blocks detection for background shading.

---

## Module 4: markdown — Rendering Engine

### MarkdownRenderer.cpp (Upgraded)
- **CSS Engine**: Premium styling inspired by ChatGPT.
- **Typography**: Inter font stack, optimized line heights (1.75).
- **Accents**: OpenAI green (`#10a37f`) for links, code blocks, and blockquotes.
- **Tables**: Smooth hover states and rounded corners.

---

## Module 5: study — Portable Annotations

### NanomarkFile.h / .cpp (New in Phase 2)
- **Format**: `.nanomark` sidecar file (JSON).
- **Content**: Sticky notes, Annotations, Bookmarks, and Reading Progress.
- **Workflow**: Portable metadata stays next to `.md` without polluting original source.

### StudyMode.cpp
- **Mode**: Read-only focus mode.
- **View**: Continuous smooth HTML rendering.

---

## Module 6: themes — Premium Design System

### dark.qss / light.qss (New in Phase 2)
- **Design System**: 4px spacing grid.
- **Palette**: Deep blacks (`#0d0d0d`), subtle borders (`#2e2e2e`), and green accents.
- **Components**: Fully styled menus, toolbars, tabs, scrollbars, and buttons.

---

## Build Instructions
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./NanoMark
```

## Binary Info
- **Size**: 9.7 MB (Phase 2 Debug)
- **Dependencies**: Qt6 (Core, Gui, Widgets, WebEngineWidgets, PrintSupport, Sql)
- **Persistence**: SQLite 3
