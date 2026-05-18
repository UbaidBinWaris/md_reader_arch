# NanoMark — Technical Architecture & Stabilization Summary

## 1. Overview & Core Mission
NanoMark is a premium, high-performance, dark-themed Markdown desktop editor and interactive study workspace built on **C++20** and **Qt 6.11**. It is engineered to bridge the gap between structured document layout, real-time interactive previews, study annotations, and publication-quality vector exports.

* **Binary**: `build/NanoMark` (~11 MB in Release mode)
* **Build System**: CMake + Make
* **Target Audience**: Researchers, students, and writers who require side-by-side editing, distraction-free reading, interactive outline tracking, and perfect print media compilation.

---

## 2. Technology Stack & Key Dependencies
* **Core Language**: C++20 (utilizing modern abstractions, structured bindings, and lambda slot captures)
* **Application Framework**: Qt 6.11 (Widgets, WebEngineWidgets, PrintSupport, Sql, Test)
* **Storage Engine**: SQLite 3 (operating in highly concurrent **Write-Ahead Logging (WAL)** mode for persistent session tracking, preferences, and workspace histories)
* **Markdown Rendering (UI)**: Custom Qt-based fast parser + QWebEngineView (Chromium-based backend)
* **PDF Compile Pipeline (Vector)**: Python 3 + WeasyPrint Layout Engine (microservice architecture for true typesetting and vector PDF rendering)
* **Design Language**: Harmonized Dark/Light OpenAI-inspired design system with thin, dynamic scrollbars and high-contrast typography

---

## 3. Directory & Module Structure

```text
src/
  core/           # Main entry point, Application initialization, WAL-optimized Settings Database
  ui/             # MainWindow layout coordination, TabManager, Dashboard welcome panel
  editor/         # One Dark styled QPlainTextEdit, live Markdown Highlighter, dynamic line offsets
  markdown/       # Markdown-to-HTML parser (incorporating GFM tables, codes blocks, and data-line tracking)
  preview/        # Persistent Chromium-based QWebEngineView, event filters, DOM update swaps
  filemanager/    # Highly responsive 200ms keystroke-debounced Autosave & Workspace restoration
  study/          # Portable .nanomark sidecar annotation model, read-only mode toggle
  themes/         # Centralized ThemeManager coordinating custom dark.qss and light.qss
  export/         # PDF & HTML ExportManager interfacing with the external Python microservice
  utils/          # High-performance Logger and runtime global Exception/Crash Handlers
microservices/
  pdf_service/    # WeasyPrint layout engine service with print-perfect publication stylesheets
resources/        # Embedded resource system containing SVG icons and QSS stylesheets
tests/            # QtTest suite validating parsing pipelines, GFM tables, and rendering tags
```

---

## 4. Upgraded Technical Architecture & Key Systems

### A. Pure Vector-Perfect PDF Exporter Pipeline
* **The Problem**: Pre-stabilization PDF exports captured rasterized, pixelated snapshots of the active QWebEngineView viewport. This caused bloated file sizes, blurry text on zoom, unselectable/unsearchable text, black margins, and poor printing quality.
* **The Architecture**:
  ```text
  Markdown Document (Raw Source)
        ↓
  Python Microservice CLI / HTTP API
        ↓
  WeasyPrint Render Engine (Typesetting & Layout)
        ↓
  Vector PDF (Selectable, Searchable, High-Resolution)
  ```
* **Implementation Details**:
  * Offloaded the PDF compile pipeline directly to Python's **WeasyPrint** layout engine, completely bypassing viewport screenshots.
  * Overhauled the print stylesheet (`PRINT_CSS` inside `microservices/pdf_service/service.py`) to enforce strict **A4 dimensions**, clean **20mm paper margins**, and custom Pygments light-background code highlighting.
  * Enforced crisp gridlines (`#b0b0b0`) for GFM tables and clearly defined borders for codeblocks to ensure excellent contrast on printed paper.

### B. Real-Time Viewport Scroll Tracking & Outline Sync (Study Mode)
* **The Problem**: In Edit Mode, the active text editor's scrollbar triggers outline highlights. However, in **Study Mode**, the text editor is hidden (`m_tabWidget->hide()`). Under Qt, hidden widgets have their rendering cycles frozen, and their scrollbar `firstVisibleBlock()` values remain static, freezing the outline selection.
* **The Architecture**:
  * **Line Attribute Compilation**: The Markdown-to-HTML parser (`src/markdown/MarkdownRenderer.cpp`) now injects a custom `data-line` attribute into every heading tag representing its exact Markdown source line number:
    `html += QString("<h%1 id='%2' data-line='%3'>%4</h%1>\n").arg(level).arg(id).arg(i + 1).arg(text);`
  * **JS Viewport Scanner**: Embedded a high-performance JavaScript viewport analyzer inside the HTML shell of the preview pane:
    ```javascript
    window.getActiveHeadingLineNumber = function() {
        var headings = document.querySelectorAll('h1, h2, h3, h4, h5, h6');
        var activeLine = -1;
        for (var i = 0; i < headings.length; i++) {
            var h = headings[i];
            var rect = h.getBoundingClientRect();
            if (rect.top <= 120) { // Scrolled past or near top of viewport
                var line = parseInt(h.getAttribute('data-line'));
                if (!isNaN(line)) activeLine = line;
            }
        }
        return activeLine;
    };
    ```
  * **Bridge Signal**: Connected `QWebEnginePage::scrollPositionChanged` in `PreviewPane.cpp` to execute the JavaScript scanner asynchronously. The returned line number is emitted via the C++ signal `headingVisibleAtTopChanged(int lineNumber)`.
  * **Highlights**: Connected this signal in `MainWindow.cpp` to selectively invoke `highlightOutlineHeadingAtLine(lineNumber)` which highlights the corresponding heading node in the tree outline panel in real-time as the user scrolls.

### C. Smooth Heading Scroll & Premium Spacing Layout
* **The Problem**: Clicking an outline item used to scroll the preview pane to an arbitrary scroll *percentage* mapped from the editor. Due to structural styling mismatches (different margins, word wrapping, and font sizes), this resulted in headings being pushed off-screen or cut off at the top.
* **The Architecture**:
  * **Modern CSS Scroll Margin**: Embedded `scroll-margin-top: 60px;` natively into the live preview CSS stylesheet for headings `h1` through `h6`.
  * **Element-Level Scroll**: Added `scrollToHeadingLine(int line)` in `PreviewPane`, which invokes the native browser scroll API on the exact heading DOM node matching the line number:
    `document.querySelector('[data-line="' + line + '"]').scrollIntoView({ behavior: 'smooth', block: 'start' });`
  * This causes the browser to smoothly glide the heading into focus, naturally reserving a gorgeous 60px margin at the top of the viewport.
  * **Editor Offset**: Adjusted the text editor's outline navigation to scroll slightly up by two lines (`qMax(0, scrollVal - step * 2)`) after executing `ensureCursorVisible()`, creating a symmetrical, clean margin in Editor Mode.

### D. Event Filter Scroll Lock/Feedback Resolution
* **The Problem**: When the user scrolled the preview pane, the `PreviewPane::eventFilter` intercepted the wheel events and delegated them to the text editor to enable synchronous scrolling. However, in Study Mode (where the editor is hidden), the editor's scrollbar has no valid layout and its scroll maximum drops to `0`. This resulted in the event filter calculating an invalid percentage and forcefully scrolling the preview pane back to the top of the page, causing jumpy, broken scroll behavior.
* **The Architecture**:
  * Updated `PreviewPane::eventFilter` to **only intercept and delegate wheel events if the editor is visible**:
    `if (editor->isVisible()) { ... delegate scroll ... }`
  * If the editor is hidden (e.g. in Study Mode), the event filter returns `false` immediately, letting the WebEngineView scroll 100% naturally using Chromium's native engine without feedback loops.

### E. Unified Sidebar Toggle & Shortcut Conflict Resolution
* **The Problem**: Having multiple QAction objects mapped to the same shortcut (e.g., `Ctrl+B`) caused Qt to throw `"Ambiguous shortcut overload: Ctrl+B"` warnings, rendering keyboard toggling non-functional.
* **The Architecture**:
  * **Centralized Method**: Created a single main window toggle handler `MainWindow::toggleSidebar()`. If either sidebar dock is visible, it hides both; if both are hidden, it restores both and raises the primary File Explorer dock.
  * **View Menu Action**: Assigned **`Ctrl+Shift+B`** globally to the View Menu's "Toggle Sidebar" action and connected it directly to `toggleSidebar()`.
  * **Toolbar Button**: Added a dedicated `📂 Sidebar` button to the main toolbar. It is connected to the same `toggleSidebar()` slot but holds **no keyboard shortcut assignment**, completely resolving duplicate ambiguous shortcut overloads in Qt.

---

## 5. Architectural Flaws, Edge Cases & Mitigations

### 1. WebEngine Initialization White Flashes & Wayland Transparencies
* **Flaw**: Under Linux Wayland compositors, initializing or displaying the `QWebEngineView` can lead to brief, blinding white flashes or layout compositor crashes due to GPU pipeline delays.
* **Mitigation**:
  * Enforced opaque painting via `m_webView->setAttribute(Qt::WA_OpaquePaintEvent, true)`.
  * Explicitly set the background color of `QWebEnginePage` to `#0d0d0d` before rendering.
  * Configured `--disable-gpu-compositing` on `QTWEBENGINE_CHROMIUM_FLAGS` in `main.cpp` to force a stable software rendering pathway on Linux environments.
  * Deferred instantiation of the WebEngineView entirely until it is first requested to save startup memory.

### 2. High-Speed Typing Thread Choking
* **Flaw**: Attempting to rebuild and compile the HTML structure on every keyboard keystroke results in main-thread UI lagging.
* **Mitigation**:
  * Embedded a highly optimized **20ms debounce single-shot timer** (`m_renderTimer`) which chokes excessive render requests.
  * Utilized high-speed Javascript DOM `innerHTML` swapping via safe JSON-escaped strings rather than re-triggering slow Chromium page reloads.

### 3. SQLite Database Write Bottlenecks on Shutdown
* **Flaw**: Restoring or saving session state variables (dozens of tabs, scroll positions, files) on close can cause heavy disk flushing, slowing down application teardown.
* **Mitigation**:
  * Configured SQLite to run in **Write-Ahead Logging (WAL)** mode.
  * Consolidated session updates into a single atomic transactional block:
    ```cpp
    QSqlDatabase::database().transaction();
    // ... write session tables ...
    QSqlDatabase::database().commit();
    ```

### 4. Headless Sandbox Environments
* **Flaw**: In specialized host sandboxes (e.g. flatpak or docker), Chromium GPU components can crash upon starting.
* **Mitigation**:
  * If Chromium initialization fails, NanoMark automatically hot-swaps to a native `QTextBrowser` widget, rendering a styled HTML document fallback cleanly without UI blackouts.

---

## 6. Build, Test & Run Pipelines

### Build Instructions
```bash
# 1. Clean and configure the build directory
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

# 2. Compile sources utilizing all CPU threads
make -j$(nproc)

# 3. Launch NanoMark
./NanoMark
```

### Test Suite Execution
NanoMark includes a unified QtTest unit-testing suite and Python microservice integration validation:
```bash
./test
```
* **C++ Tests**: Verifies markdown parsing, inline formatting, table generation, and data-line attribute rendering.
* **Python Tests**: Validates Pygments syntax rendering, HTML WeasyPrint generation, CLI export tools, and active microservice HTTP endpoint responses.
