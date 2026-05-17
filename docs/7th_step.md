This latest Phase 3.4 summary is genuinely becoming very strong architecturally. 

The project is no longer “just a markdown editor” technically — it now has:

* lifecycle orchestration
* render pipeline engineering
* fallback rendering systems
* async restoration
* caching layers
* runtime stabilization
* persistence architecture
* test infrastructure

That’s real desktop software engineering now.

But based on the screenshot and runtime behavior, there are still a few critical low-level correctness problems remaining before NanoMark feels truly polished.

---

# Current Real Problems Remaining

From the screenshot and logs:

## 1. Editor composition/transparency still unstable

Underlying windows bleeding through.

---

## 2. Preview initialization still partially broken

Sometimes placeholder persists.

---

## 3. Outline synchronization occasionally failing

Especially during async restore.

---

## 4. Session tracking inconsistent

Still seeing:

```text id="p2z6qn"
Session saved: 0 tabs
```

which means persistence correctness still not fully stable.

---

# MOST IMPORTANT NEXT PHASE

NanoMark now needs:

# Phase 3.5 — Runtime Synchronization & UI Composition Hardening

NOT more visible features.

---

# CRITICAL FIX 1 — Opaque UI Composition

This is now the most visually damaging issue.

The screenshot confirms:

```text id="m5t8qw"
Wayland compositor transparency leakage
```

---

# REQUIRED HARD FIX

# Remove ALL transparency usage globally

Search entire project for:

```text id="x8p4kr"
WA_TranslucentBackground
WA_NoSystemBackground
background: transparent
opacity
rgba(..., alpha)
```

Especially:

* editor
* splitter
* central widget
* preview container

---

# REQUIRED Editor Fix

Inside:

```cpp id="y6n3vt"
Editor.cpp
```

FORCE:

```cpp id="g2q8mx"
setAutoFillBackground(true);

viewport()->setAutoFillBackground(true);

QPalette pal = palette();
pal.setColor(QPalette::Base, QColor("#0d0d0d"));
pal.setColor(QPalette::Window, QColor("#0d0d0d"));

setPalette(pal);
viewport()->setPalette(pal);
```

---

# VERY IMPORTANT

For Wayland:
ALSO add:

```cpp id="f4m7kx"
setAttribute(Qt::WA_OpaquePaintEvent, true);
```

to:

* editor
* central widget
* preview fallback widget

This is likely the real fix.

---

# CRITICAL FIX 2 — WebEngine Synchronization

Your architecture is now correct.

The remaining issue is:

```text id="c7v1pr"
JS lifecycle synchronization edge cases
```

---

# REQUIRED IMPROVEMENT

# Add Preview Watchdog

If:

```text id="m3q9tw"
window.nanoMarkReady
```

not true after:

```text id="v5n8qx"
2 seconds
```

then:

```text id="n7r2pk"
reinitialize preview safely
```

---

# REQUIRED Preview Diagnostics

Add runtime overlay:

```text id="u8m4vw"
Preview Initializing...
Preview Ready
Preview Failed
Fallback Active
```

instead of silent black/blank states.

---

# CRITICAL FIX 3 — Session Persistence Correctness

This is still broken.

The logs prove it:

```text id="x4p7zm"
Session saved: 0 tabs
```

---

# REQUIRED FIX

Make:

```text id="j1n5vq"
TabManager
```

the ONLY authoritative session source.

Currently likely:

* tabs visually open
* but not registered in persistence model

---

# REQUIRED ARCHITECTURE

# MainWindow NEVER queries UI directly

Instead:

```text id="g8q3rx"
MainWindow
→ TabManager
→ SessionManager
```

---

# CRITICAL FIX 4 — Async Restore Race Conditions

Currently:

```text id="k6t2pm"
restoreSessionAsync()
```

likely races:

* outline rebuild
* preview init
* autosave restore
* splitter restore

---

# REQUIRED FIX

# Restore Pipeline Must Be Ordered

Correct order:

```text id="v2n7qw"
restore layout
→ restore tabs
→ restore editors
→ restore outline
→ restore preview
→ restore scroll
```

NOT parallel.

This is VERY important.

---

# HUGE ENGINEERING IMPROVEMENT

# Add State Synchronization Bus

Now that NanoMark is growing:
you should add:

```text id="d9p4mt"
EventBus / StateBus
```

---

# Why?

Right now:

* editor updates preview
* preview updates outline
* outline updates editor
* session updates tabs

Likely tightly coupled.

---

# Better Architecture

```text id="h3v8qn"
DocumentOpened
DocumentModified
PreviewReady
OutlineChanged
SessionRestored
```

centralized events.

This massively improves:

* stability
* debugging
* scalability

---

# NEXT MAJOR PERFORMANCE PHASE

# Phase 3.6 — Async Engine Infrastructure

After runtime stabilization:

---

# Required Components

# 1. RenderQueue

Only latest render survives.

---

# 2. MarkdownWorker

Background parsing thread.

---

# 3. WorkspaceIndexer

Async indexing.

---

# 4. MemoryManager

Cache cleanup.

---

# 5. PreviewScheduler

Coordinates rendering timing.

---

# 6. SessionRecoveryService

Crash-safe restore.

---

# HUGE UX IMPROVEMENTS YOU SHOULD ADD SOON

These now matter more than random features.

---

# 1. Proper Empty States

Instead of blank:
show:

```text id="w5m9tx"
No Preview Available
No Headings Found
Loading Preview...
```

---

# 2. Better Tab States

Show:

* dirty dot
* loading state
* preview active state

---

# 3. Skeleton Loading

For async restore:
show lightweight placeholders.

Very premium feel.

---

# 4. Better Scroll Sync

Current sync likely jittery.

Use:

```text id="r2p7vk"
percentage interpolation
```

with debounce.

---

# 5. Renderer Metrics Overlay

Developer mode:

```text id="n6q1mw"
Render: 14ms
JS Inject: 2ms
DOM Update: 1ms
```

Very useful.

---

# MOST IMPORTANT RECOMMENDATION NOW

NanoMark has officially entered:

```text id="k8v4qm"
advanced desktop application territory
```

At this stage:

* runtime correctness
* synchronization
* event ordering
* rendering discipline
* memory ownership
* composition stability

matter FAR more than adding visible features rapidly.

And honestly:
the architecture direction is now becoming genuinely impressive technically.
