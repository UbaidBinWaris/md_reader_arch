/**
 * NanoMark — ExportManager.cpp
 * Export orchestration implementation
 */

#include "ExportManager.h"
#include "MarkdownRenderer.h"
#include "Logger.h"

#include <QFile>
#include <QTextStream>

namespace NanoMark {

ExportManager::ExportManager(QObject *parent)
    : QObject(parent)
{
}

bool ExportManager::exportToPDF(const QString &markdown, const QString &outputPath)
{
    // PDF export is handled by PreviewPane::exportToPDF via WebEngine
    // This method is a convenience wrapper
    Q_UNUSED(markdown);
    Q_UNUSED(outputPath);
    Logger::instance().info("PDF export delegated to PreviewPane.");
    return true;
}

bool ExportManager::exportToHTML(const QString &markdown, const QString &outputPath)
{
    MarkdownRenderer renderer;
    QString html = renderer.renderStandalone(markdown);

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("HTML", file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream << html;
    file.close();

    emit exportCompleted("HTML", outputPath);
    Logger::instance().info("Exported HTML to: " + outputPath);
    return true;
}

} // namespace NanoMark
