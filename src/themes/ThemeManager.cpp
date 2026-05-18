/**
 * NanoMark — ThemeManager.cpp
 * Application-wide theme management
 */

#include "ThemeManager.h"
#include "SettingsService.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

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

QIcon ThemeManager::icon(const QString &name)
{
    QString path = QString(":/icons/%1.svg").arg(name);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QIcon();
    }
    QTextStream stream(&file);
    QString svgContent = stream.readAll();
    file.close();

    // Recolor SVG: stroke: currentColor is in all Lucide SVGs.
    // Replace "currentColor" with the exact hex color matching the active theme.
    QString colorStr = (instance().currentTheme() == Theme::Dark) ? "#ececec" : "#1f2937";
    svgContent.replace("currentColor", colorStr);

    QIcon icon;
    QByteArray svgData = svgContent.toUtf8();
    QSvgRenderer renderer(svgData);
    if (renderer.isValid()) {
        const int sizes[] = {16, 18, 20, 24, 32, 48, 64};
        for (int size : sizes) {
            QPixmap pixmap(size, size);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            renderer.render(&painter);
            icon.addPixmap(pixmap);
        }
    } else {
        icon = QIcon(path);
    }
    return icon;
}

} // namespace NanoMark
