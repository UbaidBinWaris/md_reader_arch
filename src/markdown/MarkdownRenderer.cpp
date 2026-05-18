/**
 * NanoMark — MarkdownRenderer.cpp
 * Custom markdown-to-HTML renderer supporting GFM features
 */

#include "MarkdownRenderer.h"
#include "ThemeManager.h"

#include <QRegularExpression>
#include <QStringList>
#include <QFile>

namespace NanoMark {

MarkdownRenderer::MarkdownRenderer() = default;

QString MarkdownRenderer::render(const QString &markdown) const
{
    QString htmlBody = markdownToHtml(markdown);
    QString css = getPreviewCss();

    bool isDark = ThemeManager::instance().currentTheme() == ThemeManager::Theme::Dark;

    return QString(
        "<!DOCTYPE html>"
        "<html><head>"
        "<meta charset='utf-8'>"
        "<style>%1</style>"
        "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/%2.min.css'>"
        "<script src='https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js'></script>"
        "<script>hljs.highlightAll();</script>"
        "</head><body class='%3'>%4</body></html>"
    ).arg(css,
          isDark ? "atom-one-dark" : "atom-one-light",
          isDark ? "dark" : "light",
          htmlBody);
}

QString MarkdownRenderer::renderBody(const QString &markdown) const
{
    return markdownToHtml(markdown);
}

QString MarkdownRenderer::renderStandalone(const QString &markdown) const
{
    return render(markdown);
}

// ─── Markdown → HTML Conversion ────────────────────────────────────────────────

QString MarkdownRenderer::markdownToHtml(const QString &markdown) const
{
    QStringList lines = markdown.split('\n');
    QString html;
    bool inCodeBlock = false;
    QString codeBlockLang;
    QString codeBlockContent;
    bool inList = false;
    bool inOrderedList = false;
    bool inBlockquote = false;
    bool inTable = false;
    QStringList tableHeaders;

    static QRegularExpression headingRe(R"(^(#{1,6})\s+(.+)$)");
    static QRegularExpression codeBlockStartRe(R"(^```(\w*)$)");
    static QRegularExpression codeBlockEndRe(R"(^```$)");
    static QRegularExpression ulRe(R"(^\s*[-*+]\s+(.+)$)");
    static QRegularExpression olRe(R"(^\s*\d+\.\s+(.+)$)");
    static QRegularExpression taskRe(R"(^\s*[-*+]\s+\[([ xX])\]\s+(.+)$)");
    static QRegularExpression blockquoteRe(R"(^>\s*(.*)$)");
    static QRegularExpression hrRe(R"(^(---|___|\*\*\*)\s*$)");
    static QRegularExpression tableRe(R"(^\|(.+)\|$)");
    static QRegularExpression tableSepRe(R"(^\|[\s\-:|]+\|$)");

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines[i];

        // ── Fenced code blocks ──
        if (inCodeBlock) {
            if (codeBlockEndRe.match(line).hasMatch()) {
                inCodeBlock = false;
                html += QString("<pre><code class='language-%1'>%2</code></pre>\n")
                    .arg(codeBlockLang.isEmpty() ? "plaintext" : codeBlockLang,
                         escapeHtml(codeBlockContent));
                codeBlockContent.clear();
            } else {
                codeBlockContent += line + "\n";
            }
            continue;
        }

        auto codeMatch = codeBlockStartRe.match(line);
        if (codeMatch.hasMatch()) {
            inCodeBlock = true;
            codeBlockLang = codeMatch.captured(1);
            continue;
        }

        // ── Close open lists/blockquotes if needed ──
        auto closeBlockquote = [&]() {
            if (inBlockquote) { html += "</blockquote>\n"; inBlockquote = false; }
        };
        auto closeList = [&]() {
            if (inList) { html += inOrderedList ? "</ol>\n" : "</ul>\n"; inList = false; }
        };
        auto closeTable = [&]() {
            if (inTable) { html += "</tbody></table>\n"; inTable = false; tableHeaders.clear(); }
        };

        // ── Empty line ──
        if (line.trimmed().isEmpty()) {
            closeList();
            closeBlockquote();
            closeTable();
            continue;
        }

        // ── Horizontal rule ──
        if (hrRe.match(line).hasMatch()) {
            closeList(); closeBlockquote(); closeTable();
            html += "<hr>\n";
            continue;
        }

        // ── Headings ──
        auto headingMatch = headingRe.match(line);
        if (headingMatch.hasMatch()) {
            closeList(); closeBlockquote(); closeTable();
            int level = headingMatch.captured(1).length();
            QString text = processInline(headingMatch.captured(2));
            QString id = headingMatch.captured(2).toLower()
                .replace(QRegularExpression("[^a-z0-9]+"), "-")
                .replace(QRegularExpression("^-|-$"), "");
            html += QString("<h%1 id='%2' data-line='%3'>%4</h%1>\n").arg(level).arg(id).arg(i + 1).arg(text);
            continue;
        }

        // ── Tables ──
        auto tableMatch = tableRe.match(line);
        if (tableMatch.hasMatch()) {
            // Check if next line is separator
            if (!inTable && i + 1 < lines.size() && tableSepRe.match(lines[i + 1]).hasMatch()) {
                closeList(); closeBlockquote();
                inTable = true;
                QStringList cells = tableMatch.captured(1).split('|');
                html += "<table><thead><tr>";
                for (const QString &cell : cells) {
                    tableHeaders.append(cell.trimmed());
                    html += "<th>" + processInline(cell.trimmed()) + "</th>";
                }
                html += "</tr></thead><tbody>\n";
                ++i; // Skip separator line
                continue;
            }
            if (inTable) {
                QStringList cells = tableMatch.captured(1).split('|');
                html += "<tr>";
                for (const QString &cell : cells) {
                    html += "<td>" + processInline(cell.trimmed()) + "</td>";
                }
                html += "</tr>\n";
                continue;
            }
        }

        // ── Blockquotes ──
        auto bqMatch = blockquoteRe.match(line);
        if (bqMatch.hasMatch()) {
            closeList(); closeTable();
            if (!inBlockquote) {
                html += "<blockquote>\n";
                inBlockquote = true;
            }
            html += "<p>" + processInline(bqMatch.captured(1)) + "</p>\n";
            continue;
        }

        // ── Task lists ──
        auto taskMatch = taskRe.match(line);
        if (taskMatch.hasMatch()) {
            closeBlockquote(); closeTable();
            if (!inList) {
                html += "<ul class='task-list'>\n";
                inList = true;
                inOrderedList = false;
            }
            bool checked = taskMatch.captured(1).toLower() == "x";
            html += QString("<li class='task-list-item'><input type='checkbox' disabled %1> %2</li>\n")
                .arg(checked ? "checked" : "", processInline(taskMatch.captured(2)));
            continue;
        }

        // ── Unordered lists ──
        auto ulMatch = ulRe.match(line);
        if (ulMatch.hasMatch()) {
            closeBlockquote(); closeTable();
            if (!inList || inOrderedList) {
                if (inList) html += "</ol>\n";
                html += "<ul>\n";
                inList = true;
                inOrderedList = false;
            }
            html += "<li>" + processInline(ulMatch.captured(1)) + "</li>\n";
            continue;
        }

        // ── Ordered lists ──
        auto olMatch = olRe.match(line);
        if (olMatch.hasMatch()) {
            closeBlockquote(); closeTable();
            if (!inList || !inOrderedList) {
                if (inList) html += "</ul>\n";
                html += "<ol>\n";
                inList = true;
                inOrderedList = true;
            }
            html += "<li>" + processInline(olMatch.captured(1)) + "</li>\n";
            continue;
        }

        // ── Paragraph ──
        closeList(); closeBlockquote(); closeTable();
        html += "<p>" + processInline(line) + "</p>\n";
    }

    // Close any remaining open blocks
    if (inCodeBlock) {
        html += QString("<pre><code>%1</code></pre>\n").arg(escapeHtml(codeBlockContent));
    }
    if (inList) html += inOrderedList ? "</ol>\n" : "</ul>\n";
    if (inBlockquote) html += "</blockquote>\n";
    if (inTable) html += "</tbody></table>\n";

    return html;
}

// ─── Inline Processing ─────────────────────────────────────────────────────────

QString MarkdownRenderer::processInline(const QString &line) const
{
    QString result = escapeHtml(line);

    // Images: ![alt](url)
    static QRegularExpression imgRe(R"(!\[([^\]]*)\]\(([^\)]+)\))");
    result.replace(imgRe, R"(<img src='\2' alt='\1' style='max-width:100%'>)");

    // Links: [text](url)
    static QRegularExpression linkRe(R"(\[([^\]]+)\]\(([^\)]+)\))");
    result.replace(linkRe, R"(<a href='\2'>\1</a>)");

    // Bold: **text** or __text__
    static QRegularExpression boldRe1(R"(\*\*([^\*]+)\*\*)");
    result.replace(boldRe1, R"(<strong>\1</strong>)");
    static QRegularExpression boldRe2(R"(__([^_]+)__)");
    result.replace(boldRe2, R"(<strong>\1</strong>)");

    // Italic: *text* or _text_
    static QRegularExpression italicRe1(R"(\*([^\*]+)\*)");
    result.replace(italicRe1, R"(<em>\1</em>)");
    static QRegularExpression italicRe2(R"(_([^_]+)_)");
    result.replace(italicRe2, R"(<em>\1</em>)");

    // Strikethrough: ~~text~~
    static QRegularExpression strikeRe(R"(~~([^~]+)~~)");
    result.replace(strikeRe, R"(<del>\1</del>)");

    // Inline code: `code`
    static QRegularExpression codeRe(R"(`([^`]+)`)");
    result.replace(codeRe, R"(<code>\1</code>)");

    return result;
}

QString MarkdownRenderer::escapeHtml(const QString &text) const
{
    QString result = text;
    result.replace('&', "&amp;");
    result.replace('<', "&lt;");
    result.replace('>', "&gt;");
    return result;
}

// ─── Heading Extraction ────────────────────────────────────────────────────────

QVector<Heading> MarkdownRenderer::extractHeadings(const QString &markdown) const
{
    QVector<Heading> headings;
    QStringList lines = markdown.split('\n');
    static QRegularExpression headingRe(R"(^(#{1,6})\s+(.+)$)");

    for (int i = 0; i < lines.size(); ++i) {
        auto match = headingRe.match(lines[i]);
        if (match.hasMatch()) {
            headings.append({
                static_cast<int>(match.captured(1).length()),
                match.captured(2),
                i + 1
            });
        }
    }

    return headings;
}

// ─── Preview CSS ───────────────────────────────────────────────────────────────

QString MarkdownRenderer::getPreviewCss() const
{
    return R"(
        * { margin: 0; padding: 0; box-sizing: border-box; }

        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif;
            font-size: 15px;
            line-height: 1.75;
            padding: 32px 40px;
            max-width: 860px;
            margin: 0 auto;
            -webkit-font-smoothing: antialiased;
        }

        body.dark {
            background: #0d0d0d;
            color: #ececec;
        }

        body.light {
            background: #ffffff;
            color: #1a1a1a;
        }

        h1, h2, h3, h4, h5, h6 {
            margin-top: 1.6em;
            margin-bottom: 0.6em;
            font-weight: 700;
            line-height: 1.3;
            letter-spacing: -0.02em;
            scroll-margin-top: 60px;
        }

        body.dark h1, body.dark h2, body.dark h3 { color: #ffffff; }
        body.light h1, body.light h2, body.light h3 { color: #000000; }

        h1 { font-size: 2em; border-bottom: 1px solid rgba(255,255,255,0.1); padding-bottom: 0.4em; }
        h2 { font-size: 1.5em; }
        h3 { font-size: 1.25em; }

        body.light h1 { border-bottom-color: #e5e5e5; }

        p { margin: 0.8em 0; }

        a { color: #10a37f; text-decoration: none; font-weight: 500; }
        a:hover { text-decoration: underline; }

        code {
            font-family: 'JetBrains Mono', 'Fira Code', 'Cascadia Code', monospace;
            font-size: 0.88em;
            padding: 3px 7px;
            border-radius: 5px;
            font-weight: 500;
        }

        body.dark code { background: #1a1a1a; color: #10a37f; border: 1px solid #2e2e2e; }
        body.light code { background: #f5f5f5; color: #0d8c6d; border: 1px solid #e5e5e5; }

        pre {
            margin: 1.2em 0;
            padding: 20px;
            border-radius: 10px;
            overflow-x: auto;
            line-height: 1.6;
        }

        body.dark pre { background: #111111; border: 1px solid #2e2e2e; }
        body.light pre { background: #f8f8f8; border: 1px solid #e5e5e5; }

        pre code {
            padding: 0;
            background: none;
            border: none;
            font-size: 0.85em;
            color: inherit;
        }

        blockquote {
            margin: 1.2em 0;
            padding: 12px 20px;
            border-radius: 6px;
        }

        body.dark blockquote {
            border-left: 3px solid #10a37f;
            background: rgba(16,163,127,0.06);
            color: #a1a1a1;
        }

        body.light blockquote {
            border-left: 3px solid #10a37f;
            background: rgba(16,163,127,0.04);
            color: #666666;
        }

        ul, ol { margin: 0.8em 0; padding-left: 2em; }
        li { margin: 0.4em 0; }

        .task-list { list-style: none; padding-left: 0; }
        .task-list-item { display: flex; align-items: center; gap: 8px; padding: 2px 0; }
        .task-list-item input { margin: 0; transform: scale(1.15); accent-color: #10a37f; }

        table {
            border-collapse: collapse;
            margin: 1.2em 0;
            width: 100%;
            border-radius: 8px;
            overflow: hidden;
        }

        th, td { padding: 10px 16px; text-align: left; }

        body.dark th { background: #171717; border: 1px solid #2e2e2e; color: #ececec; font-weight: 600; }
        body.dark td { border: 1px solid #1a1a1a; }
        body.light th { background: #f5f5f5; border: 1px solid #e5e5e5; color: #1a1a1a; font-weight: 600; }
        body.light td { border: 1px solid #f0f0f0; }

        body.dark tr:hover td { background: rgba(16,163,127,0.04); }
        body.light tr:hover td { background: rgba(16,163,127,0.03); }

        hr {
            margin: 2em 0;
            border: none;
            height: 1px;
        }

        body.dark hr { background: #2e2e2e; }
        body.light hr { background: #e5e5e5; }

        img { max-width: 100%; border-radius: 10px; margin: 1.2em 0; }
        strong { font-weight: 650; }
        del { opacity: 0.5; text-decoration: line-through; }

        body.dark ::selection { background: rgba(16,163,127,0.3); }
        body.light ::selection { background: rgba(16,163,127,0.2); }
    )";
}

} // namespace NanoMark
