#pragma once

/**
 * NanoMark — ThemeManager.h
 * Application-wide theme management
 */

#include <QObject>
#include <QString>
#include <QIcon>

namespace NanoMark {

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class Theme {
        Dark,
        Light
    };

    /** Get the singleton instance */
    static ThemeManager& instance();

    /** Load and apply a theme */
    void loadTheme(Theme theme);

    /** Get current theme */
    Theme currentTheme() const { return m_currentTheme; }

    /** Get preview CSS for current theme */
    QString previewCssPath() const;

    /** Get themed SVG icon by name */
    static QIcon icon(const QString &name);

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager();
    Theme m_currentTheme = Theme::Dark;
};

} // namespace NanoMark
