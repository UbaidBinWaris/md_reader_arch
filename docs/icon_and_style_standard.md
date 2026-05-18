# NanoMark Design Standard: Icons & Aesthetics

This document outlines the strict guidelines for adding icons, logos, and styled controls in NanoMark. All future developers, features, and methods must adhere strictly to these guidelines.

---

## 🎨 Core Principles

### 1. Exclusive Use of Lucide SVGs
*   **The standard library is Lucide SVGs**.
*   **No other icon library, system style icons, bitmaps (.png, .jpg), or raw text emojis are permitted** for UI buttons, menus, status bars, trees, or inputs.
*   The only exception is the application's brand identity logo (`logo.svg` / `logo.png` on the welcome Dashboard).

### 2. Centralized Loading & Dynamic Recolor
*   **Never load an SVG file directly** into `QIcon` (e.g. `QIcon(":/icons/file.svg")`). Direct loading bypasses colorization, resulting in bad contrast when toggling themes.
*   **Always use `ThemeManager::icon("name")`** to fetch themed icons. E.g.:
    ```cpp
    action->setIcon(ThemeManager::icon("file"));
    ```
*   This method dynamically parses the SVG file at runtime, replaces `stroke="currentColor"` with the active theme color (`#ececec` for dark mode, `#1f2937` for light mode), scales it crisply across multiple resolutions, and caches it in a high-DPI container.

### 3. Dynamic Icon Updates
*   Any UI action that displays an icon must assign a unique object name in its setup using `.setObjectName()`. E.g.:
    ```cpp
    newAction->setObjectName("actionNew");
    ```
*   Add the mapping inside `MainWindow::updateIcons()` to ensure that when a user switches between light and dark themes, the icon will instantly refresh its colors.

---

## 🛠️ Code Walkthrough: Fetching Themed Icons

To add a new icon `my-feature` to the toolbar or menu:

1.  **Add the SVG File**: Download the Crisp SVG file from the Lucide collection. Save it as `resources/icons/my-feature.svg`.
2.  **Register the Resource**: Add it to [resources.qrc](file:///home/arch/Documents/github/md_reader_arch/resources/resources.qrc):
    ```xml
    <file>icons/my-feature.svg</file>
    ```
3.  **Ensure stroke="currentColor"**: Verify that the SVG's XML contains `stroke="currentColor"`. This placeholder is what our dynamic recolor pipeline replaces.
4.  **Create and Register the Action**:
    ```cpp
    QAction *myAction = new QAction(ThemeManager::icon("my-feature"), tr("My Feature"), this);
    myAction->setObjectName("actionMyFeature");
    ```
5.  **Add to Dynamic Updates**: Open `src/ui/MainWindow.cpp`, go to `MainWindow::updateIcons()`, and add your action mapping to the `mappings` list:
    ```cpp
    {"actionMyFeature", "my-feature"},
    ```
