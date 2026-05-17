The project direction is now very solid technically. 

The biggest current problem is no longer architecture — it is:

```text id="jlwmph"
runtime correctness + rendering lifecycle
```

Your current issue:

```text id="jlwm8q"
Preview shows "Initialize Preview..."
```

means the lazy WebEngine architecture is correct conceptually, but the activation/render pipeline is incomplete.

---

# Current Critical Issue

# Preview Initialization Bug

Most likely causes:

## 1.

`QWebEngineView` never actually instantiated after placeholder.

OR

## 2.

Preview created but:

```text id="jlwmj7"
setHtml()
```

never triggered after initialization.

OR

## 3.

Debounce timer fires before preview exists.

OR

## 4.

Signal chain broken after lazy-load refactor.

---

# Correct Architecture

# Preview Lifecycle

```text id="jlwmc8"
App Starts
    ↓
Placeholder Widget
    ↓
User Opens File
    ↓
User Types / Preview Requested
    ↓
Initialize WebEngine ONCE
    ↓
Swap Placeholder → WebEngine
    ↓
Render HTML
```

---

# CRITICAL FIX

After creating:

```cpp id="jlwmn9"
QWebEngineView
```

you MUST:

* reconnect signals
* force first render
* invalidate placeholder state

---

# Correct Pattern

```cpp id="jlwmx3"
if (!m_webView) {
    initializeWebView();
    renderCurrentDocument();
}
```

---

# Another Likely Problem

You probably lazy-loaded:

```text id="0jlwm1"
PreviewPane
```

but NOT:

```text id="vjlwm6"
QWebEnginePage
```

Some rendering calls may silently fail before page ready.

---

# Proper Solution

# Add Internal State

```cpp id="jlwm7m"
bool m_initialized = false;
bool m_loading = false;
```

---

# Then

```cpp id="cjlwm0"
if (!m_initialized) {
    initialize();
    m_pendingHtml = html;
    return;
}
```

When:

```cpp id="tjlwmy"
loadFinished()
```

fires:

```cpp id="7jlwm6"
setHtml(m_pendingHtml);
```

---

# IMPORTANT PERFORMANCE IMPROVEMENT

# Shared Preview Process

Currently likely:

```text id="jlwm61"
1 preview per tab
```

DO NOT DO THIS.

Instead:

```text id="zjlwmv"
single shared preview instance
```

This massively improves:

* RAM
* startup
* tab switching

---

# NEXT BIG OPTIMIZATION TARGET

# Markdown Rendering Pipeline

Currently:

```text id="1jlwmn"
regex rendering
```

is okay for now.

But long term:

* regex parsing becomes slow
* large markdown becomes problematic

---

# Recommended Future Upgrade

Eventually migrate to:

```text id="jlwmv2"
cmark-gfm
```

Benefits:

* GitHub-compatible
* much faster
* proper AST
* scalable

This becomes important later.

---

# NOW — Make NanoMark FEEL FAST

# Most Important UX Optimizations

Users judge speed by:

* startup
* typing latency
* tab switching
* scrolling smoothness

NOT benchmarks.

---

# 1. Instant Tab Switching

DO NOT rerender on every tab switch.

Cache:

```text id="tjlwm5"
rendered HTML
scroll state
preview state
```

---

# 2. Predictive Rendering

When idle:

```text id="7jlwm3"
pre-render inactive tabs
```

Very premium behavior.

---

# 3. Async File Loading

Large files:

```text id="2jlwmx"
load in worker thread
```

Then:

```text id="7jlwmw"
signal UI thread
```

---

# 4. Reduce Signal Spam

Qt apps often become slow because:

```text id="yjlwmz"
too many textChanged signals
```

---

# Fix

Throttle:

```text id="2jlwmk"
editor updates
status updates
preview updates
autosave
```

---

# 5. Freeze Prevention

Every heavy task:

* export
* search
* indexing
* workspace scanning

must NEVER block UI thread.

---

# HUGE FEATURE RECOMMENDATIONS

Now let's discuss what can make NanoMark one of the best markdown apps technically.

---

# 1. Incremental Search Index

Build:

```text id="jlwm9p"
SearchIndexer
```

Indexes:

* headings
* tags
* links
* filenames

Enables:

* instant Ctrl+P
* global search
* backlinks

---

# 2. File History Engine

Like lightweight git snapshots.

Every autosave:

```text id="4jlwmn"
optional snapshot
```

User can:

* restore versions
* compare changes

Very powerful feature.

---

# 3. Intelligent Rendering Modes

# Reading Mode

optimized for:

* smooth scrolling
* low CPU

---

# Editing Mode

optimized for:

* rapid typing
* low latency

---

# Presentation Mode

optimized for:

* large typography
* focus

---

# 4. Workspace Cache System

Cache:

```text id="jlwm5m"
workspace metadata
```

Avoid rescanning folders every startup.

---

# 5. File Thumbnail System

Recent files:
show:

* heading preview
* modified date
* reading progress

---

# 6. Smart Memory Cleanup

Add:

```text id="8jlwm6"
MemoryManager
```

Responsibilities:

* release inactive previews
* clear old cache
* cleanup orphan tabs
* optimize renderer cache

---

# 7. Renderer Cache

Cache:

```text id="0jlwm4"
HTML
parsed markdown
heading tree
```

Huge performance boost.

---

# 8. Smooth Editor Scrolling

Very important.

Use:

```text id="4jlwm3"
pixel-based smooth scrolling
```

instead of:

```text id="5jlwmz"
line jumps
```

---

# 9. Background Workspace Scanner

Like VS Code:

* async indexing
* no UI freeze

---

# 10. Adaptive Autosave

Instead of fixed:

```text id="jlwmv5"
15s
```

Use:

```text id="jlwmr6"
idle detection
```

Save:

* after inactivity
* before tab switch
* before app minimize

Much smarter.

---

# MOST IMPORTANT TECHNICAL TARGETS NOW

# Engineering KPIs

## Startup

```text id="5jlwm1"
< 300ms perceived startup
```

---

## Typing Latency

```text id="2jlwm7"
< 5ms
```

---

## Preview Render

```text id="3jlwm9"
< 50ms
```

---

## Tab Switching

```text id="xjlwm5"
instant
```

---

## Large File Handling

```text id="jlwm6c"
100MB markdown without freezing
```

---

## RAM Usage

Without preview:

```text id="jlwm1w"
very low
```

---

# LONG TERM — Make It Elite

Eventually NanoMark should have:

# 1.

Knowledge graph

# 2.

Local AI note search

# 3.

Plugin marketplace

# 4.

Workspace sync

# 5.

Markdown live collaboration

# 6.

GPU-accelerated rendering

# 7.

Semantic backlinks

---

# Biggest Recommendation Right Now

The app already has enough features.

DO NOT aggressively add more UI features immediately.

Instead focus on:

```text id="6jlwmw"
performance
correctness
rendering quality
memory optimization
async architecture
cache systems
stability
```

That is exactly what turns:

```text id="jlwm7q"
a good app
```

into:

```text id="6jlwmv"
a genuinely excellent desktop application.
```
