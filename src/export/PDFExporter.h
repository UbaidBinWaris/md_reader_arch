#pragma once

/**
 * NanoMark — PDFExporter.h
 * PDF export helper
 */

#include <QString>

namespace NanoMark {

class PDFExporter
{
public:
    /** Export HTML content to PDF file */
    static bool exportToPDF(const QString &html, const QString &outputPath);
};

} // namespace NanoMark
