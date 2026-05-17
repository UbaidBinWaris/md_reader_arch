NanoMark is now reaching the stage where the biggest differentiator will be:

```text id="3z0n6h"
engineering quality
```

not feature count.

Your current Phase 3.1 architecture is already becoming quite strong technically. 

Now the focus should become:

* performance engineering
* memory optimization
* rendering optimization
* lifecycle stability
* async architecture
* file indexing performance
* startup speed
* low RAM usage
* smooth UX
* crash resistance

The goal is:

```text id="22qvfm"
Best native markdown reader/editor experience
```

---

# NanoMark — Optimization & Stability Roadmap

# Core Engineering Philosophy

NanoMark should feel:

```text id="2qf8eg"
instant
lightweight
smooth
predictable
responsive
stable
```

Like:

* SumatraPDF startup speed
* VS Code workflow
* Obsidian readability
* native Linux responsiveness

WITHOUT:

* Electron lag
* memory leaks
* UI freezes
* heavy GPU usage

---

# Priority 1 — Memory Management

# CRITICAL RULE

Qt apps become slow mainly because of:

* widget over-creation
* hidden WebEngine instances
* uncontrolled signals
* large document repainting
* memory fragmentation
* unnecessary copies

---

# Required Memory Architecture

# 1. Use Smart Pointers Everywhere

Replace raw ownership with:

```cpp id="hq4jnd"
std::unique_ptr
std::shared_ptr
QPointer
```

---

# Recommended Ownership Rules

## unique_ptr

For:

* services
* managers
* isolated ownership

---

## shared_ptr

ONLY when truly shared.

Avoid overusing it.

---

## QPointer

For:

* QObject tracking
* destroyed-widget safety

---

# 2. Avoid Widget Recreation

DO NOT:

```text id="sn0mhy"
recreate preview widgets
recreate editors
rebuild layouts repeatedly
```

Instead:

```text id="smwvy1"
reuse components
```

---

# 3. Shared Preview Engine

Very important.

Instead of:

```text id="9d6dcd"
one QWebEngineView per tab
```

Use:

```text id="1g6q1f"
single reusable PreviewPane
```

Massive RAM reduction.

---

# 4. Virtual Document Loading

Large markdown files:

```text id="jlwmmg"
100MB+
```

must NOT fully load into rendering pipeline instantly.

---

# Recommended

Implement:

```text id="d39mb6"
chunked rendering
```

Later:

* incremental parsing
* visible-region rendering

---

# 5. Background Parsing Threads

Move markdown parsing off UI thread.

Use:

```cpp id="jlwm3d"
QtConcurrent::run()
QThreadPool
```

---

# Example Async Pipeline

```text id="jlwm5p"
Editor Change
    ↓
Debounce Timer
    ↓
Background Parse
    ↓
UI Thread Render
```

This is CRITICAL.

---

# Priority 2 — Rendering Performance

# Current Bottleneck

Most likely:

```text id="vkt8ya"
QWebEngine
```

---

# Optimization Strategy

# 1. Debounced Rendering

Already added:

```text id="64n5ny"
250ms
```

Good.

Can dynamically adapt:

* fast typing → 400ms
* idle → instant

---

# 2. Render Diffing

Instead of:

```text id="4stjzj"
rerender entire document
```

Later implement:

```text id="1lfjlwm"
partial updates
```

Huge improvement.

---

# 3. Cache HTML Output

Store:

```text id="jlwm6t"
last rendered HTML
```

Avoid redundant rendering.

---

# 4. Scroll Sync Optimization

Use:

```text id="jlwmqm"
percentage-based sync
```

not pixel sync.

Much smoother.

---

# Priority 3 — SQLite Optimization

# Current Architecture Good

But improve further.

---

# 1. Enable WAL Mode

VERY important.

On startup:

```sql id="jlwm13"
PRAGMA journal_mode=WAL;
```

Benefits:

* faster writes
* less blocking
* safer autosave

---

# 2. Prepared Statements

Never:

```text id="jlwm81"
string-concatenate SQL
```

Always:

```cpp id="jlwmfu"
QSqlQuery::prepare()
```

---

# 3. Batch Transactions

For session saves:

```sql id="jlwmu6"
BEGIN TRANSACTION;
```

Huge speed improvement.

---

# 4. Database Cleanup Service

Add:

```text id="jlwmvv"
DatabaseMaintenanceService
```

Tasks:

* vacuum
* cleanup old sessions
* remove stale autosaves

---

# Priority 4 — File System Performance

# Workspace Indexer

Future critical feature.

Build:

```text id="kzjlwm"
IndexerService
```

---

# Responsibilities

## Background File Indexing

Store:

* headings
* links
* tags
* metadata

---

# Async File Watcher

Use:

```cpp id="jlwmkg"
QFileSystemWatcher
```

for:

* file changes
* external edits
* deleted files

---

# Recommended

Throttle filesystem events.

---

# Priority 5 — Startup Speed

Goal:

```text id="jlwm2m"
< 500ms perceived startup
```

---

# Startup Optimization Strategy

# 1. Lazy Load Everything

DO NOT initialize:

* plugins
* preview
* workspace index
* exporters

until needed.

---

# 2. Async Dashboard Loading

Dashboard appears instantly.

Recent files load afterward.

---

# 3. Deferred WebEngine Startup

Huge optimization.

Only create:

```text id="jlwm0r"
QWebEngineView
```

when preview first needed.

Massive startup improvement.

---

# Priority 6 — Stability Engineering

# 1. Global Crash Handler

Already partially added.

Improve with:

```text id="wjlwmu"
CrashRecoveryService
```

---

# Save On Crash

Automatically:

* active tabs
* unsaved docs
* session state

---

# 2. Safe File Writes

VERY important.

Current atomic writes are good.

Improve further:

```text id="y0mjlwm"
write → flush → fsync → rename
```

---

# 3. Corrupted Session Recovery

If DB corrupted:

* auto backup
* rebuild database
* preserve documents

---

# Priority 7 — Advanced Technical Features

# 1. Memory Usage Monitor

Add developer mode diagnostics:

```text id="jjlwm5"
RAM
CPU
Render Time
Parse Time
```

Very useful.

---

# 2. Performance Profiler

Internal timing system:

```cpp id="2njlwm"
ScopedTimer
```

Measure:

* render time
* markdown parse time
* DB operations

---

# 3. Async Export Pipeline

PDF export should NEVER freeze UI.

---

# 4. GPU Feature Toggle

Allow:

```text id="vjlwm4"
disable GPU rendering
```

for stability on Linux.

---

# Priority 8 — Make It Feel Premium

# Features That Matter MOST

---

# 1. Command Palette

CRITICAL feature.

---

# 2. Quick Open

Fuzzy search:

```text id="cjlwm1"
Ctrl+P
```

---

# 3. Workspace Symbol Search

Search headings globally.

---

# 4. Split Editors

Multiple views of same file.

---

# 5. Live Scroll Sync

Editor ↔ Preview.

---

# 6. Breadcrumb Navigation

---

# 7. Markdown Table GUI

---

# 8. Image Optimization

Compress pasted images automatically.

---

# 9. Embedded Terminal

Optional developer mode panel.

---

# 10. Vim Mode

Huge power-user feature.

---

# Long-Term Killer Features

# 1. Knowledge Graph

Like Obsidian.

---

# 2. AI Semantic Search

Later:

* embeddings
* local AI
* note linking

---

# 3. Plugin Marketplace

---

# 4. Real-Time Collaboration

---

# 5. Portable Workspace Bundles

```text id="6jlwm6"
.nanomark-workspace
```

---

# Testing Strategy Upgrade

# You NEED

# 1. Unit Tests

Already started.

Expand heavily.

---

# 2. Integration Tests

Critical for:

* autosave
* restore
* export
* workspace

---

# 3. Stress Tests

Test:

* 100 tabs
* huge markdown
* rapid typing
* autosave spam

---

# 4. Memory Leak Testing

Use:

```text id="jlwm0n"
Valgrind
AddressSanitizer
```

VERY important.

---

# 5. Performance Benchmarks

Benchmark:

* startup
* render
* search
* save
* export

---

# MOST IMPORTANT RECOMMENDATION

DO NOT chase feature quantity anymore.

NanoMark already has:

* good architecture
* SQLite
* session management
* modern rendering
* modularity
* autosave
* study mode
* plugin foundations

Now the difference between:

```text id="9jlwm7"
good project
```

and:

```text id="2jlwmn"
elite desktop application
```

is:

* responsiveness
* polish
* stability
* memory efficiency
* workflow quality
* rendering smoothness
* startup speed
* consistency

That is the stage NanoMark has entered now.
