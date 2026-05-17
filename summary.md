# NanoMark — Technical Summary (Phase 3.4 Stabilization & Startup Optimized)

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
- **Status Bar Upgrades**: Dynamic "Read Time" (200 wpm calculation) and "File Size" formatting (KB/MB) running in real-time.
- Contextually hides Line/Col information in Study Mode.
- **Startup Recovery**: Bypasses dialog prompts to **always restore recovery sessions automatically** on startup, backed by try-catch fallback structures to open a clean workspace on database/file read failures.
- **Interactive Outline & Scroll Sync**: Coordinates two-way scroll synchronization, promptless startup recoveries, and active reading section tracking. Exposes the viewport-top scroll alignment logic (`setValue(maximum())` followed by `ensureCursorVisible()`) to scroll editor text perfectly to the top edge on heading double-clicks.
- **View Layout State Persistence**: Saves the exact visibility state of the preview pane (`isPreviewVisible`) in SQLite and restores it on startup, resolving startup layout glitches.
- **Lockstep Toggle Synchronizations**: Exposes checkable menu actions (`m_toggleStudyAction`, `m_togglePreviewAction`) and binds their check states in absolute phase-lockstep with backend variables, enabling `Ctrl + E` to toggle Study Mode instantly on the very first keystroke after starting in editor-only mode.

---

## Module 3: editor — Premium Text Editing

### Editor.cpp
- **Theme**: Harmonized with deep black `#0d0d0d` background.
- **Line Numbers**: Dynamic coloring (highlighted current line).
- **Behavior**: Auto-indentation, list continuation, 4-space tabs.
- **Public API**: Exposes `firstVisibleBlock()` publicly to support active outline section reading tracking.

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

### Preview Performance (Phase 3.4 Stabilization & Caching)
- **Persistent HTML Shell & innerHTML Swap**: Instead of calling `setHtml()` repeatedly, which reloads Chromium entirely and causes white flashes, `PreviewPane` bootstraps a static HTML shell *once*. Subsequent rendering updates are executed using high-speed JavaScript `innerHTML` swaps via safe `QJsonDocument` string serialization. This preserves DOM states, scroll positions, and reduces CPU rendering overhead to near zero.
- **Timing-Safe JavaScript Handshake**: Handled race conditions by checking `window.nanoMarkReady === true` via a C++ deferred polling handshake loop inside `onLoadFinished()`. Guarantees JS functions are fully parsed and live before content injection starts.
- **Global JS Scope Fix**: Exposed the swap functions directly on the `window.updateContent` namespace, completely resolving WebEngine scope isolation issues.
- **QTextBrowser Fallback Engine**: If Chromium fails, crashes, or is unavailable in the host sandbox environment, rendering automatically hot-swaps to a native, theme-harmonized `QTextBrowser` widget, preventing blank preview states.
- **Robust Exception Logging**: Connected a global `window.onerror` catch block in the JS shell to route runtime scripting errors directly to developer logs via a `runJavaScript` logging callback.
- **Opaque Dark Paint Override**: Completely eliminated white rendering flashes. Sets `Qt::WA_OpaquePaintEvent` false, overrides Chromium's background directly to `#0d0d0d`, and embeds immediate CSS background styling into the bootstrap shell.
- **Wayland Software Renderer Fallback**: Configured `--disable-gpu-compositing` on `QTWEBENGINE_CHROMIUM_FLAGS` in `main.cpp` to prevent transparent compositor bleed-through crashes on Linux Wayland.
- **Deferred Initialization**: The heavy Chromium WebEngine is instantiated *only* when the preview is opened/visible, cutting idle memory footprint drastically.
- **Blazing-Fast Debounced Rendering**: Debounce interval is configured to **`20ms`** (equaling 50 FPS!), which delivers incredibly fluid, instant, and lag-free real-time typing preview updates without any visual delay.
- **Study Mode Full-Width Rendering**: Completely decoupled rendering loops from `m_isStudyMode` checks, allowing users to read their compiled documents in elegant distraction-free full-width Study views.
- **Instant Tab Switching (HTML Cache)**: Each tab's `Editor` caches its rendered HTML body. If the document hasn't changed (i.e. scrolling, cursor moves), NanoMark serves the cached HTML instantly (`< 1ms`) upon switching tabs.
- **Ready-Gated Cache Verification & Handshake Clear**: Solved initialization caching bugs by gating HTML comparison checks on `m_state == PreviewState::Ready`, and proactively clearing `m_lastHtmlBody` right before executing handshake pending updates, ensuring document-load injections always run flawlessly.
- **Offline Highlight Safety**: Added checks to prevent JS failures if highlight.js CDN stylesheets are offline.

- **Asynchronous Session Restoration & Autorestore**: Bypassed dialog prompts to **always restore** recovery documents automatically on startup. Protected by try-catch blocks to safely open a clean workspace on read failures.
- **Continuous Document Outline**: Pulled heading generation outside of cache dirty checks so outlines rebuild instantly on tab swap and cold boot. Shows a dim/italic `"No headings found"` placeholder row when headers are empty.
- **Interactive Outline Navigation**: Connected the `QTreeView` outline items to scroll the editor by block-by-line using stored `lineNumber` and native `QTextBlock` APIs. Clicking a heading now scrolls it perfectly to the very top edge of the editor viewport via a robust `setValue(maximum())` followed by `ensureCursorVisible()` scroll-up alignment strategy (bypassing word-wrapped scrollbar unit offsets), while automatically ensuring that the clicked outline tree item itself is highlighted and scrolled to in the left sidebar tree view.
- **Active Section Reading Tracking**: Dynamically tracks the visible section at the very top of the editor viewport (`firstVisibleBlock()`) and automatically highlights/scrolls to that heading in the left Outline sidebar in real-time as the user types or scrolls through their document.
- **Real-Time Outline Search Filter**: Integrated a premium, dark-themed `QLineEdit` search bar at the top of the Outline sidebar widget, using a high-performance `QSortFilterProxyModel` to filter outline headings instantaneously as the user types.
- **Symmetric Two-Way Scroll Sync**: Synchronizes the vertical scroll position of both the Editor and HTML Preview panel bidirectionally. Scrolling on either pane will scroll the other in perfect real-time unison.
- **Single-Scrollbar Cleanliness**: Hid the redundant second scrollbar on the preview pane (`::-webkit-scrollbar { display: none; }` and `setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff)`) to present a single, sleek, unified scroll layout.
- **Premium Stylings**: Integrated opaque editor composition overlays, raised text contrast to `#f8f9fa` for superb readability, added dark-contrasted selected tab active states, and enforced solid `#2a2a2a` borders throughout the global `dark.qss` layout.

---

## Module 5: filemanager — Autosave & File I/O

### AutosaveManager.h / .cpp (Overhauled in Phase 3.4)
- **Keystroke-Debounced Instant Autosave**: Replaced 15-second background timers with a highly responsive `200ms` single-shot debounce timer. The editor now automatically saves your modifications to the `.autosave` recovery file immediately as you type (triggered instantly 200ms after you pause).
- **Tracking**: Correctly tracks and saves unsaved "Untitled" documents. Connects to `QPlainTextEdit::textChanged`.
- **Metadata**: Embeds original file paths directly inside the backup file for seamless restoration.
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
