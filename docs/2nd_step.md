Perfect.
Now the architecture direction is becoming very clear and much more professional.

Your decisions move NanoMark from a simple editor into a real extensible desktop productivity platform.

---

# NanoMark — Finalized Architecture Direction

# Window Management System

## Default Behavior

```text id="66r6mw"
Single Main Window + Multi Tabs
```

Like:

* VS Code
* Chrome
* Obsidian

This keeps:

* memory usage lower
* workflow cleaner
* session restoration easier

---

## Advanced Behavior

If user explicitly selects:

```text id="jzc0ql"
"Open in New Window"
```

Then:

* create secondary application window
* independent tab context
* separate workspace state
* separate preview/editor state

---

# Window System Architecture

## Core Components

```text id="4gljz6"
WindowManager
 ├── MainWindow
 ├── SecondaryWindow
 ├── TabManager
 └── SessionManager
```

---

# Workspace System

# Final Decision

```text id="hngw49"
Workspace support = YES
```

But:

```text id="r7em4q"
NEVER automatically open folders.
```

This is a very good UX decision.

---

# Workspace Workflow

User manually chooses:

* Open Folder
* Add Workspace
* Open Project

from:

* home screen
* file menu
* command palette

---

# Workspace Features

## Sidebar Explorer

Like VS Code:

* nested folders
* collapsible tree
* markdown icons
* drag & drop

---

## Sidebar Controls

User can:

* hide/show sidebar
* resize sidebar
* move panels
* dock panels
* restore layout

---

# Dockable Layout System

Qt supports this very well using:

```cpp id="vkrzzn"
QDockWidget
```

---

# User Layout Features

## User Can:

* move sidebar left/right
* detach panels
* collapse preview
* resize panels
* restore default layout

---

# Final Study Mode Design

# Continuous HTML Rendering

Final decision:

```text id="16iq2d"
NO page-cut rendering.
```

Instead:

```text id="3kqksj"
Continuous smooth HTML document rendering.
```

Like:

* web page
* Notion
* Obsidian preview
* ChatGPT reading experience

---

# Benefits

## Better:

* scrolling
* responsiveness
* annotations
* sticky notes
* dynamic resizing

---

# Sticky Notes & Comments Architecture

# Final Decision

```text id="s06mx2"
Metadata remains separate from original markdown.
```

Excellent decision.

This prevents:

* markdown corruption
* compatibility issues
* markdown pollution

---

# New Requirement Added

You introduced a very important feature:

```text id="r5z7ko"
Portable NanoMark annotations.
```

---

# Solution

When user shares markdown:

They can optionally export:

```text id="3hx4pz"
.md + .nanomark
```

Example:

```text id="tvz44q"
notes.md
notes.nanomark
```

---

# .nanomark File

This file contains:

* comments
* highlights
* sticky notes
* bookmarks
* reading progress
* annotations

Stored as:

```text id="vwq8md"
JSON or SQLite package
```

---

# Sharing Workflow

## User A

Creates notes/comments.

## User B

Opens `.md`.

NanoMark detects:

```text id="g8odc4"
matching .nanomark metadata
```

Then automatically restores:

* sticky notes
* comments
* highlights

WITHOUT modifying original markdown.

This is extremely professional architecture.

---

# Plugin & Extension Architecture

# Final Decision

```text id="tjlwm0"
YES — Plugin system planned early.
```

Excellent decision.

This changes architecture significantly.

---

# Why This Matters

Now NanoMark becomes:

```text id="0dxk1t"
an extensible platform
```

instead of:

```text id="t5mvlf"
a fixed markdown app.
```

---

# Recommended Plugin Architecture

# Core Application

Minimal stable core:

* editor
* preview
* tabs
* rendering
* settings
* workspace

---

# Plugin Layer

External modules can add:

* commands
* themes
* exporters
* renderers
* AI tools
* study utilities

---

# Recommended Plugin Structure

```text id="24yejf"
plugins/
 ├── ai-assistant/
 ├── mermaid-tools/
 ├── latex-tools/
 ├── export-enhancer/
 └── vim-mode/
```

---

# Plugin API

Later expose:

```cpp id="5o1b1n"
PluginInterface
CommandInterface
RendererInterface
ThemeInterface
```

using:

```cpp id="54f74u"
Qt Plugin System
```

---

# Microservices Direction

# Important Clarification

For desktop app:

```text id="f3i7mq"
DO NOT over-engineer into true distributed microservices early.
```

Instead use:

```text id="e5h1j5"
Modular Service-Oriented Architecture
```

inside the app first.

---

# Recommended Architecture

# Internal Services

```text id="42g8u8"
EditorService
PreviewService
WorkspaceService
ExportService
ThemeService
SettingsService
AnnotationService
SessionService
PluginService
```

Each isolated and independent.

---

# Why This Is Better

Advantages:

* easier testing
* cleaner code
* easier future migration
* plugin ready
* possible future IPC

WITHOUT:

* unnecessary complexity
* performance overhead

---

# Future Migration Path

Later if needed:

* cloud sync service
* collaboration service
* AI service
* plugin marketplace

can become:

* local services
* remote APIs
* websocket services

---

# Updated App Philosophy

NanoMark should now feel like:

```text id="v2gzn8"
VS Code + Obsidian + MS Word Reading Mode
```

with:

```text id="7mjlwm"
SumatraPDF performance philosophy
```

---

# Next Major Engineering Goals

# Immediate Priority Order

# 1. UI/UX Foundation Rewrite

## Build:

* OpenAI-inspired dark theme
* typography system
* spacing system
* icon system
* animations
* hover states

This is now the MOST important task.

---

# 2. Settings & Persistence Layer

Build:

* SQLite manager
* settings service
* session restore
* recent files
* workspace state

---

# 3. Professional Window System

Implement:

* tab manager
* layout manager
* dockable panels
* sidebar system

---

# 4. Study Mode Engine

Build:

* read-only workflow
* annotations
* sticky notes
* comments overlay
* focus mode

---

# 5. Workspace System

Build:

* explorer tree
* file search
* workspace metadata
* folder indexing

---

# 6. Plugin Architecture Foundation

Even if plugins come later:

* create interfaces now
* avoid tightly coupled systems

---

# Very Important Future Decision

You will eventually need:

## Rendering Engine Decision

Choose later between:

* QTextDocument renderer
  OR
* full HTML engine

Honestly:

```text id="62o1sx"
QWebEngine-based HTML rendering
```

is probably the best long-term direction.

Because:

* modern rendering
* CSS styling
* smooth scrolling
* future collaboration features
* better annotations
* web-like study mode

---

# Final Refined Product Direction

NanoMark is becoming:

```text id="zyv5wp"
A native cross-platform markdown productivity workspace
focused on study, development, documentation,
and professional reading workflows.
```

with:

* native desktop performance
* modular architecture
* extensibility
* modern UI/UX
* workspace support
* annotation ecosystem
* professional persistence layer
* future plugin ecosystem

This is now a genuinely serious software engineering project.
