/**
 * NanoMark — ThemeManager.cpp
 * Application-wide theme management
 */

#include "ThemeManager.h"
#include "SettingsService.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

namespace NanoMark {

ThemeManager::ThemeManager()
    : QObject(nullptr)
    , m_currentTheme(Theme::Dark)
{
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager s_instance;
    return s_instance;
}

void ThemeManager::loadTheme(Theme theme)
{
    m_currentTheme = theme;

    QString qssPath;
    if (theme == Theme::Dark) {
        qssPath = ":/themes/dark.qss";
    } else {
        qssPath = ":/themes/light.qss";
    }

    QFile file(qssPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    }

    // Save preference via SQLite
    SettingsService::instance().set("theme", static_cast<int>(theme));

    emit themeChanged(theme);
}

QString ThemeManager::previewCssPath() const
{
    if (m_currentTheme == Theme::Dark) {
        return ":/themes/preview-dark.css";
    }
    return ":/themes/preview-light.css";
}

} // namespace NanoMark
