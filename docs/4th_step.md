Phase 3 is already moving in the right direction. 

Right now the biggest priority is not adding random features quickly — it is stabilizing:

* session management
* autosave
* dock/layout state
* rendering pipeline
* UI consistency

The logs you shared are actually very useful because they expose the exact architectural issues that need fixing first.

---

# Current Critical Issues

# 1. Autosave Not Working

This is the MOST important issue now.

Your architecture says:

```text id="ngcz1v"
AutosaveManager exists
```

but logs show:

```text id="b4rbd8"
Session saved: 0 tabs
```

This means:

* tabs are not registered properly
  OR
* editor state is not being tracked
  OR
* autosave manager is not connected to tab lifecycle

---

# Root Cause Likely

The autosave system probably:

* only checks existing saved file paths
* ignores untitled/new tabs
* does not connect editor `textChanged`
* does not maintain dirty-state registry

---

# Correct Architecture

# AutosaveManager MUST Track

```text id="jlwmfb"
documentId
tabId
filePath
isModified
lastSaved
autosavePath
```

---

# Required Fix

# Every Editor Tab Needs

```cpp id="93y7oo"
connect(editor, &QPlainTextEdit::textChanged,
        autosaveManager, &AutosaveManager::markDirty);
```

---

# Autosave Timer

Every:

```text id="jlwmmk"
15–30 seconds
```

check:

```cpp id="jzzvli"
if(document->isModified())
```

then autosave.

---

# Critical Missing Feature

Untitled documents.

Example:

```text id="b6rj4r"
Untitled-1.md
```

must autosave too.

---

# Recommended Autosave Structure

## Autosave Folder

```text id="yj5kph"
~/Documents/NanoMark/Autosave/
```

---

# Autosave File Names

Use:

```text id="sq7k5q"
UUID + metadata
```

NOT only hashes.

Example:

```text id="s27q1q"
session_8f2a1.md.autosave
```

---

# Restore Workflow

On app reopen:

Show:

```text id="vmfscd"
"Recover autosaved files?"
```

like:

* VS Code
* MS Word

---

# 2. saveState() Warnings

This is VERY important.

Logs:

```text id="dh35iy"
QMainWindow::saveState():
'objectName' not set
```

This breaks:

* dock restoration
* toolbar restoration
* layout persistence

---

# Fix

ALL:

* QDockWidget
* QToolBar
* Splitters

MUST have:

```cpp id="i4lbdh"
setObjectName("UniqueName");
```

Example:

```cpp id="7v0uh9"
sidebarDock->setObjectName("SidebarDock");
toolbar->setObjectName("MarkdownToolbar");
```

Without this:

```text id="jlwm5d"
restoreState() cannot work properly.
```

This is likely why:

* layout restoration broken
* sidebar state broken
* app reopening incorrectly

---

# 3. Vulkan Errors

Logs:

```text id="jrn7p1"
Failed to create Vulkan instance
```

Usually harmless with Qt WebEngine on Linux.

---

# Recommended Fix

Add before QApplication:

```cpp id="79gn9x"
qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
        "--disable-gpu --disable-vulkan");
```

OR:

```cpp id="z6psh8"
QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
```

---

# 4. Main Dashboard Problems

You mentioned:

```text id="yz1vll"
main page design not good
```

This is expected.

Right now it likely feels:

* empty
* generic
* old Qt UI

---

# Professional Dashboard Direction

# Design Goal

Mix:

* VS Code welcome page
* ChatGPT spacing
* Notion minimalism

---

# Dashboard Layout

```text id="0hnl98"
+--------------------------------------+
| NanoMark                             |
| "Write. Study. Organize."            |
+--------------------------------------+

[ New File ]
[ Open File ]
[ Open Folder ]

Recent Files
----------------------------------------
• AI Notes.md
• Research.md
• Assignment.md

Pinned
----------------------------------------
• Thesis.md

Tips / Shortcuts
----------------------------------------
Ctrl+P → Quick Open
Ctrl+Shift+P → Commands
```

---

# Important UX Improvement

Use:

```text id="t2pnyj"
large spacing
rounded cards
hover animations
minimal borders
```

Avoid:

```text id="i2rw5m"
old Qt desktop style
```

---

# 5. GUI Consistency Problems

This is VERY common in Qt apps.

---

# Common Issues

Usually caused by:

* mixed margins
* inconsistent padding
* native widget styles leaking
* inconsistent font sizing
* multiple conflicting QSS rules

---

# Solution

Create:

```text id="jlwm6m"
Design System
```

---

# Design System Rules

# Global Spacing

```text id="1wh7f0"
4px base system
```

Example:

```text id="u1q9x6"
4
8
12
16
24
32
```

ONLY use these.

---

# Radius System

```text id="jlwmzj"
6px
8px
12px
```

No random values.

---

# Typography Scale

```text id="4j9jlwm"
12px small
14px body
16px heading
20px large heading
```

---

# Recommended New Features (High Value)

Now let's discuss features that ACTUALLY improve the app professionally.

---

# Priority Features

# 1. Quick Open (VERY Important)

Like:

```text id="jlwmma"
Ctrl+P
```

Instant fuzzy file search.

Massive productivity improvement.

---

# 2. Command Palette

Like:

```text id="aen7lf"
Ctrl+Shift+P
```

Actions:

* toggle study mode
* export
* themes
* open folder
* zoom
* switch tabs

---

# 3. File Outline Navigation

Sidebar:

```text id="z3zjlwm"
# Heading
## Subheading
```

Clickable navigation.

---

# 4. Global Search

Search:

* files
* headings
* sticky notes
* annotations

---

# 5. Markdown Templates

Templates:

* notes
* assignment
* research
* meeting notes
* README

---

# 6. Distraction Free Mode

Hide:

* tabs
* sidebar
* statusbar
* toolbar

---

# 7. Split Preview Directions

Allow:

```text id="jlwmj3"
horizontal
vertical
```

preview layouts.

---

# 8. Image Drag & Drop

Drag image into editor:

```text id="5wwhjlwm"
auto copy + markdown insert
```

---

# 9. File History / Snapshots

Simple local versioning.

Very powerful feature.

---

# 10. Workspace Symbol Search

Like VS Code:

```text id="jlwm1d"
search headings globally
```

---

# MOST IMPORTANT NEXT STEP

DO THIS ORDER:

# Phase 3.1 Stabilization

FIRST FIX:

* autosave
* saveState warnings
* session restoration
* dashboard design
* scrollbar consistency
* layout restoration

---

# THEN

# Phase 3.2 Productivity

ADD:

* command palette
* quick open
* templates
* outline navigation

---

# THEN

# Phase 3.3 Premium Polish

ADD:

* animations
* icon system
* font system
* transitions
* blur/effects

---

# Biggest Recommendation

NanoMark is now reaching:

```text id="8l6lcb"
real software territory
```

So now:

```text id="jlwm1z"
stability > features
```

for the next development stage.
