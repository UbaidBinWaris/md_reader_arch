#pragma once

/**
 * NanoMark — HTMLExporter.h
 * Standalone HTML export
 */

#include <QString>

namespace NanoMark {

class HTMLExporter
{
public:
    /** Export markdown to standalone HTML file */
    static bool exportToHTML(const QString &markdown, const QString &outputPath);
};

} // namespace NanoMark
