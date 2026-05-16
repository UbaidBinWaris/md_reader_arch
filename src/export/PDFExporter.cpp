/**
 * NanoMark — PDFExporter.cpp
 * PDF export via QPrinter
 */

#include "PDFExporter.h"
#include "Logger.h"

namespace NanoMark {

bool PDFExporter::exportToPDF(const QString &html, const QString &outputPath)
{
    // PDF export is primarily handled through QWebEnginePage::printToPdf
    // in PreviewPane. This class is a placeholder for future non-WebEngine
    // PDF export (e.g., using QPrinter with QTextDocument).
    Q_UNUSED(html);
    Q_UNUSED(outputPath);
    Logger::instance().info("PDFExporter: export requested");
    return true;
}

} // namespace NanoMark
