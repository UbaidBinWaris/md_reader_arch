# NanoMark — Project Summary

## Overview

NanoMark is a modern, lightweight, high-performance markdown editor and study workspace built using C++ and Qt 6.

The application combines:

* professional markdown editing
* developer-focused workflows
* live preview rendering
* study and annotation tools
* multi-tab document management
* export functionality
* native desktop performance

The goal is to create a native desktop application similar to:

* VS Code (layout/workflow)
* SumatraPDF (speed/lightweight behavior)
* Obsidian (markdown productivity)
* MS Word View Mode (study/reading mode)

while maintaining:

* low memory usage
* fast startup
* clean architecture
* professional UX
* cross-platform support

---

# Primary Goals

## Main Goal

Create a professional markdown desktop application that is:

```text id="mc13bq"
- Fast
- Lightweight
- Native
- Modern
- Extensible
- Developer Friendly
- Study Friendly
```

---

# Core Objectives

## 1. Native Performance

* Built fully in C++
* Qt 6 native widgets
* minimal RAM usage
* fast rendering
* smooth scrolling
* responsive UI

---

## 2. Professional Markdown Editing

Support advanced markdown editing with:

* syntax highlighting
* toolbar formatting
* live preview
* split editor
* multi-tab editing
* keyboard shortcuts

---

## 3. Study & Reading Workspace

Provide a distraction-free study environment with:

* view-only mode
* sticky notes
* comments/annotations
* section highlights
* reading focus tools

---

## 4. Cross Platform

Support:

* Linux
* Windows
* macOS

Future:

* Android
* iOS

---

## 5. Extensible Architecture

Allow future support for:

* plugins
* AI tools
* cloud sync
* collaboration
* workspace systems

---

# Application Modes

# 1. Developer Mode

## Purpose

Focused on markdown writing and technical documentation.

## Features

### Split Layout

```text id="lz6n58"
Left  -> Markdown Source Editor
Right -> Live Preview
```

---

### Developer Features

* syntax highlighting
* line numbers
* markdown toolbar
* code block support
* table generator
* keyboard shortcuts
* auto save
* live rendering
* tab system
* command palette
* search & replace

---

### Markdown Toolbar

Buttons for:

* Heading
* Bold
* Italic
* Underline
* Lists
* Checklists
* Tables
* Links
* Images
* Code blocks
* Quotes
* Horizontal rules

---

### Advanced Features

* Mermaid diagrams
* LaTeX rendering
* GitHub flavored markdown
* HTML preview
* export tools

---

# 2. Study Mode

## Purpose

Focused on reading, reviewing, and note-taking.

## Features

### View Only Mode

Like MS Word reading mode.

Capabilities:

* disable editing
* smooth reading
* clean UI
* zoom controls
* focus mode

---

### Annotation System

Users can:

* add sticky notes
* highlight sections
* add comments
* create study reminders
* attach notes to headings

---

### Study Tools

* reading progress
* bookmarks
* focus reading mode
* quick navigation
* document outline
* collapsible headings

---

# System Architecture

# High-Level Architecture

```text id="wd7gls"
+------------------------------------------------+
|                 NanoMark UI                    |
+------------------------------------------------+
| Tabs | Sidebar | Editor | Preview | Notes      |
+------------------------------------------------+
| File System | Markdown Engine | Export Engine  |
+------------------------------------------------+
| Theme Engine | Settings | Memory Manager       |
+------------------------------------------------+
```

---

# Project Modules

# 1. Core Application Module

## Responsibilities

* app startup
* window management
* initialization
* event loop
* global configuration

## Files

```text id="gg4vab"
main.cpp
Application.cpp
Application.h
```

---

# 2. Main Window Module

## Responsibilities

* main UI layout
* dock widgets
* toolbar management
* menu system
* status bar

## Files

```text id="dpkv4v"
MainWindow.cpp
MainWindow.h
```

---

# 3. Editor Engine Module

## Responsibilities

* markdown editing
* syntax highlighting
* text formatting
* cursor operations
* keyboard shortcuts

## Features

* line numbers
* auto indentation
* smart formatting
* undo/redo

## Files

```text id="i3rydl"
Editor.cpp
Editor.h
MarkdownHighlighter.cpp
MarkdownHighlighter.h
```

---

# 4. Markdown Rendering Module

## Responsibilities

* markdown parsing
* HTML rendering
* live preview updates

## Features

* tables
* code highlighting
* diagrams
* LaTeX support

## Files

```text id="0xmw63"
MarkdownRenderer.cpp
MarkdownRenderer.h
PreviewPane.cpp
PreviewPane.h
```

---

# 5. File Management Module

## Responsibilities

* open/save files
* workspace management
* recent files
* auto recovery

## Features

* drag & drop
* multi-tab handling
* file watcher

## Files

```text id="i67rf7"
FileManager.cpp
FileManager.h
WorkspaceManager.cpp
WorkspaceManager.h
```

---

# 6. Study Mode Module

## Responsibilities

* view mode
* sticky notes
* annotations
* reading features

## Files

```text id="w7a7eh"
StudyMode.cpp
StudyMode.h
StickyNotes.cpp
StickyNotes.h
Annotations.cpp
Annotations.h
```

---

# 7. Export Engine Module

## Responsibilities

Export markdown to:

* PDF
* HTML
* DOCX

## Files

```text id="w94jnk"
ExportManager.cpp
ExportManager.h
PDFExporter.cpp
HTMLExporter.cpp
DOCXExporter.cpp
```

---

# 8. Theme Engine Module

## Responsibilities

* dark/light mode
* custom themes
* UI styling

## Files

```text id="aqm4vd"
ThemeManager.cpp
ThemeManager.h
```

---

# 9. Keyboard Shortcut System

## Responsibilities

Professional productivity shortcuts.

## Examples

```text id="7cm31d"
Ctrl + N        -> New File
Ctrl + O        -> Open File
Ctrl + S        -> Save
Ctrl + Shift+S  -> Save As
Ctrl + P        -> Quick Open
Ctrl + F        -> Find
Ctrl + H        -> Replace
Ctrl + Shift+P  -> Command Palette
Ctrl + B        -> Bold
Ctrl + I        -> Italic
```

---

# 10. Error Handling Module

## Goals

Prevent crashes and corruption.

## Features

* safe file operations
* exception handling
* logging system
* backup recovery
* corrupted file protection

## Files

```text id="s6o8yh"
ErrorHandler.cpp
ErrorHandler.h
Logger.cpp
Logger.h
```

---

# 11. Memory Management Module

## Goals

Maintain high performance.

## Features

* smart pointers
* optimized rendering
* lazy loading
* background processing
* cache cleanup

## Techniques

```text id="49rmpr"
- std::unique_ptr
- std::shared_ptr
- RAII
- move semantics
- worker threads
```

---

# UI Components

# Main UI Sections

## 1. Sidebar

Contains:

* file explorer
* outline
* bookmarks
* workspace files

---

## 2. Editor Panel

Markdown editing area.

---

## 3. Preview Panel

Live rendered markdown.

---

## 4. Sticky Notes Panel

Study annotations.

---

## 5. Status Bar

Displays:

* line number
* encoding
* file type
* word count

---

# Folder Structure

```text id="j7sv5z"
NanoMark/
│
├── src/
│   ├── core/
│   ├── editor/
│   ├── markdown/
│   ├── preview/
│   ├── study/
│   ├── export/
│   ├── themes/
│   ├── filemanager/
│   ├── ui/
│   └── utils/
│
├── resources/
│   ├── icons/
│   ├── logo/
│   ├── themes/
│   └── fonts/
│
├── docs/
├── tests/
├── build/
└── CMakeLists.txt
```

---

# Performance Targets

```text id="amwqeo"
Startup Time:
< 1 second

RAM Usage:
< 150 MB

Large File Support:
100MB+ markdown files

Smooth Scrolling:
60 FPS target

Crash Recovery:
Auto backup every few minutes
```

---

# Logo Direction

## Style

Arch Linux inspired modern logo.

## Design Elements

* markdown symbol
* minimal geometric shape
* blue/cyan palette
* developer aesthetic

---

# Future Roadmap

# Version 2

* plugin system
* AI assistant
* cloud sync
* git integration
* Vim mode
* minimap
* terminal panel

---

# Version 3

* mobile companion app
* collaboration mode
* workspace sync
* real-time editing

---

# Recommended Tech Stack

```text id="uy02dg"
Language:
C++20

Framework:
Qt 6

Build System:
CMake

Markdown Engine:
QTextDocument + custom parser

Export:
Qt PDF + external libraries

Architecture:
Modular MVC-inspired architecture
```

---

# Development Philosophy

NanoMark should feel:

* professional
* extremely responsive
* distraction free
* developer oriented
* stable
* memory efficient
* keyboard driven
* visually clean

while remaining:

* lightweight
* maintainable
* scalable
* extensible for future growth.
