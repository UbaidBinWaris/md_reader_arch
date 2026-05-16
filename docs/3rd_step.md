NanoMark is now at the stage where the focus should shift from:

```text id="pj7mjr"
"adding features"
```

to:

```text id="1xj1a3"
"refining experience, performance, and architecture quality"
```

Your current Phase 2 structure is already very strong. The next improvements should make the application feel:

* premium
* modern
* lightweight
* polished
* stable
* fast

like a real production-grade desktop application.

---

# Priority 1 — Make NanoMark Lightweight

# Biggest Performance Rule

Avoid:

```text id="kl6g2j"
heavy repainting
full markdown rerender every keystroke
blocking UI thread
unnecessary Qt widgets
```

---

# Recommended Optimizations

# 1. Debounced Rendering

DO NOT render preview instantly every keypress.

Instead:

```text id="yhs9gw"
250ms debounce timer
```

Example:

```cpp id="9pndm8"
QTimer* previewTimer;
previewTimer->setSingleShot(true);
previewTimer->start(250);
```

Huge performance improvement.

---

# 2. Lazy Preview Updates

If:

* tab inactive
* preview hidden
* study mode active

then:

```text id="q60f57"
pause rendering updates
```

---

# 3. Use Worker Threads

Move:

* markdown parsing
* exports
* indexing

to:

```text id="6q9h5r"
QThreadPool
QtConcurrent
```

Keeps UI smooth.

---

# 4. Reduce WebEngine Usage

Qt WebEngine is heavy.

Recommended:

* one shared preview engine
* avoid multiple hidden preview instances
* reuse renderer

---

# 5. Avoid Huge QSS Repaints

Large QSS updates:

```text id="93vjlwm"
can lag UI heavily
```

Only update theme:

* once
* at startup
* on manual switch

---

# Priority 2 — Professional Scrollbar System

YES — your observation is correct.

Default Qt scrollbars:

```text id="9w4n34"
look old and ugly
```

---

# Desired Scrollbar Behavior

Like:

* ChatGPT
* Linear
* VS Code modern overlays

---

# Scrollbar Design

## Default State

```text id="zuwkpv"
thin
dark
minimal
semi-transparent
```

---

## Hover State

```text id="lnnkkx"
expand width
increase contrast
smooth animation
```

---

# Recommended Sizes

## Normal

```text id="tx3zhu"
width: 6px
```

## Hover

```text id="uwh2m3"
width: 10px
```

---

# Recommended QSS

```css id="3n8r3d"
QScrollBar:vertical {
    background: transparent;
    width: 6px;
    margin: 0px;
}

QScrollBar::handle:vertical {
    background: #2d2d2d;
    border-radius: 3px;
    min-height: 30px;
}

QScrollBar::handle:vertical:hover {
    background: #4a4a4a;
}
```

---

# Border Glow Problem

You mentioned:

```text id="j86nl8"
white outline/glow border issue
```

Likely caused by:

* default Qt focus border
* QDockWidget focus
* WebEngine focus ring

---

# Fix

Add globally:

```css id="d11vvb"
*:focus {
    outline: none;
}
```

and:

```css id="mrdnq6"
border: none;
```

for:

* dock widgets
* editor
* splitter
* preview

---

# Icons — VERY Important

Your instinct is correct.

Current icons:

```text id="n6m3nx"
feel outdated
```

---

# Best Direction

You said:

```text id="4oj40w"
React Icons style
```

Excellent choice.

Modern apps use:

* outline icons
* minimal icons
* rounded icons
* monochrome icons

---

# Recommended Icon Packs

Use SVG only.

Best choices:

* Lucide
* Heroicons
* Tabler Icons
* Remix Icons

---

# Recommended Style

```text id="t98alj"
minimal
2px stroke
monochrome
modern
clean
```

---

# PNG vs SVG

## Use:

```text id="ek9nm7"
SVG for UI
```

Advantages:

* scalable
* crisp
* themeable
* lightweight

---

# Recommended Icon System

Create:

```text id="7pcu0z"
IconManager
```

Handles:

* theme switching
* SVG loading
* dynamic coloring
* caching

---

# Testing Strategy (VERY Important)

YES — now you need proper testing.

---

# Recommended Testing Structure

```text id="t3s97m"
tests/
 ├── editor/
 ├── markdown/
 ├── filemanager/
 ├── settings/
 ├── ui/
 └── integration/
```

---

# Use QtTest

Recommended:

```text id="42exbp"
Qt6::Test
```

---

# What To Test

# Unit Tests

## MarkdownRenderer

Test:

* headings
* tables
* code blocks
* inline formatting

---

## FileManager

Test:

* save
* autosave
* restore
* corrupted file handling

---

## SettingsService

Test:

* SQLite writes
* session restore
* recent files

---

# Integration Tests

Test:

* opening workspace
* restoring tabs
* autosave recovery
* export pipeline

---

# UI Tests

Test:

* tab switching
* split panels
* theme switching

---

# Autosave System (Final Design)

# Required Behavior

When:

```text id="es3x7j"
new file opens
```

Automatically:

* create temp autosave
* save to Documents/NanoMark

---

# Recommended Path

## Linux

```text id="d2wlhx"
~/Documents/NanoMark/Autosave/
```

## Windows

```text id="vr1fgh"
Documents/NanoMark/Autosave/
```

---

# Autosave Architecture

## Two Types

# 1. Session Autosave

Temporary recovery state.

# 2. Real Autosave

Actual file overwrite.

User configurable.

---

# Session Restoration (Very Important)

YES — absolutely.

Like:

* VS Code
* Chrome
* Obsidian

---

# Required Restore State

When app reopens:

* exact tabs
* exact scroll positions
* cursor positions
* study/edit mode
* sidebar state
* panel sizes
* active workspace

---

# Footer Improvements

# Study Mode Footer

Correct decision:

```text id="uhocd2"
hide cursor info
```

Because:

* no editing
* cleaner reading experience

---

# Footer Should Show

## Study Mode

```text id="trjlwm"
Read Time
Word Count
File Size
Zoom
Encoding
Markdown Type
```

---

# File Size

YES.

Show:

```text id="vwjx1h"
KB / MB
```

Example:

```text id="t2rnwo"
1.4 MB
```

Very professional touch.

---

# Open App Workflow

# New Desired UX

When app launches:

## If Previous Session Exists

Restore exactly.

---

## If No Session Exists

Show:

```text id="r5u3y4"
Welcome Dashboard
```

Contains:

* recent files
* pinned notes
* quick actions
* open folder
* new markdown file

---

# Additional HIGH VALUE Features

Now these are the BEST features you can add.

---

# 1. Command Palette

Like:

```text id="3k7qxl"
Ctrl+Shift+P
```

Huge professional improvement.

---

# 2. Global Search

Search:

* workspace
* headings
* sticky notes
* comments

---

# 3. Minimap

Optional editor minimap like VS Code.

---

# 4. File Outline Navigation

Clickable headings sidebar.

---

# 5. Reading Time Estimation

Example:

```text id="b7u7dg"
8 min read
```

---

# 6. Zen Mode

Hide:

* sidebar
* footer
* tabs

For deep reading/writing.

---

# 7. Smooth Animations

Subtle:

* panel resize
* hover
* transitions
* tab switching

---

# 8. Workspace Search Indexing

Later:

* fast file search
* symbol search
* fuzzy finder

---

# 9. Markdown Templates

Templates for:

* notes
* documentation
* assignments
* research

---

# 10. AI Assistant Layer (Future)

Potentially:

* summarize markdown
* rewrite
* explain notes
* generate docs

---

# MOST IMPORTANT NEXT TASKS

# TOP PRIORITY

## 1.

Fix visual polish:

* borders
* scrollbar
* icons
* spacing
* typography

---

## 2.

Complete session persistence.

---

## 3.

Professional autosave/recovery.

---

## 4.

Build Welcome Dashboard.

---

## 5.

Create testing infrastructure.

---

# Biggest Long-Term Recommendation

At this point:

```text id="aqvbmq"
DO NOT add random features quickly.
```

Instead:
focus on:

* polish
* consistency
* responsiveness
* architecture quality

That is what separates:

```text id="l1x6x5"
a student project
```

from:

```text id="6yrzzk"
a professional desktop application.
```
