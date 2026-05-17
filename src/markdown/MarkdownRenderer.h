#pragma once

/**
 * NanoMark — MarkdownRenderer.h
 * Converts markdown text to styled HTML
 */

#include <QString>
#include <QVector>

namespace NanoMark {

struct Heading {
    int level;
    QString text;
    int lineNumber;
};

class MarkdownRenderer
{
public:
    MarkdownRenderer();

    /** Render markdown to HTML fragment (for preview pane) */
    QString render(const QString &markdown) const;

    /** Render markdown body content only */
    QString renderBody(const QString &markdown) const;

    /** Render markdown to complete standalone HTML document */
    QString renderStandalone(const QString &markdown) const;

    /** Extract heading structure for outline */
    QVector<Heading> extractHeadings(const QString &markdown) const;

    /** Get the current preview styling CSS */
    QString getPreviewCss() const;

private:
    QString markdownToHtml(const QString &markdown) const;
    QString escapeHtml(const QString &text) const;
    QString processInline(const QString &line) const;
    QString getHighlightJsCdn() const;
};

} // namespace NanoMark
