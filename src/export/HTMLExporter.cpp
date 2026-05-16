/**
 * NanoMark — HTMLExporter.cpp
 * Standalone HTML export
 */

#include "HTMLExporter.h"
#include "MarkdownRenderer.h"
#include "Logger.h"

#include <QFile>
#include <QTextStream>

namespace NanoMark {

bool HTMLExporter::exportToHTML(const QString &markdown, const QString &outputPath)
{
    MarkdownRenderer renderer;
    QString html = renderer.renderStandalone(markdown);

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::instance().error("HTMLExporter: Failed to open " + outputPath);
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << html;
    file.close();

    Logger::instance().info("HTMLExporter: Exported to " + outputPath);
    return true;
}

} // namespace NanoMark
