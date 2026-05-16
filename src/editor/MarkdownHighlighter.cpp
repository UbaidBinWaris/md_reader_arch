/**
 * NanoMark — MarkdownHighlighter.cpp
 * Syntax highlighter for markdown source editing
 */

#include "MarkdownHighlighter.h"
#include <QTextDocument>

namespace NanoMark {

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // ── Heading formats ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(97, 175, 239));   // blue
        fmt.setFontWeight(QFont::Bold);
        fmt.setFontPointSize(18);
        m_rules.push_back({QRegularExpression(R"(^# .+$)"), fmt});

        QTextCharFormat fmt2;
        fmt2.setForeground(QColor(97, 175, 239));
        fmt2.setFontWeight(QFont::Bold);
        fmt2.setFontPointSize(16);
        m_rules.push_back({QRegularExpression(R"(^## .+$)"), fmt2});

        QTextCharFormat fmt3;
        fmt3.setForeground(QColor(97, 175, 239));
        fmt3.setFontWeight(QFont::Bold);
        fmt3.setFontPointSize(14);
        m_rules.push_back({QRegularExpression(R"(^### .+$)"), fmt3});

        QTextCharFormat fmt456;
        fmt456.setForeground(QColor(97, 175, 239));
        fmt456.setFontWeight(QFont::Bold);
        m_rules.push_back({QRegularExpression(R"(^#{4,6} .+$)"), fmt456});
    }

    // ── Bold ──
    {
        QTextCharFormat fmt;
        fmt.setFontWeight(QFont::Bold);
        fmt.setForeground(QColor(224, 108, 117));   // red
        m_rules.push_back({QRegularExpression(R"(\*\*[^\*]+\*\*)"), fmt});
        m_rules.push_back({QRegularExpression(R"(__[^_]+__)"), fmt});
    }

    // ── Italic ──
    {
        QTextCharFormat fmt;
        fmt.setFontItalic(true);
        fmt.setForeground(QColor(152, 195, 121));   // green
        m_rules.push_back({QRegularExpression(R"((?<!\*)\*(?!\*)[^\*]+\*(?!\*))"), fmt});
        m_rules.push_back({QRegularExpression(R"((?<!_)_(?!_)[^_]+_(?!_))"), fmt});
    }

    // ── Strikethrough ──
    {
        QTextCharFormat fmt;
        fmt.setFontStrikeOut(true);
        fmt.setForeground(QColor(130, 130, 130));
        m_rules.push_back({QRegularExpression(R"(~~[^~]+~~)"), fmt});
    }

    // ── Inline code ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(229, 192, 123));   // yellow
        fmt.setBackground(QColor(50, 50, 50));
        fmt.setFontFamilies({"JetBrains Mono"});
        m_rules.push_back({QRegularExpression(R"(`[^`]+`)"), fmt});
    }

    // ── Links ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(86, 182, 194));    // cyan
        fmt.setFontUnderline(true);
        m_rules.push_back({QRegularExpression(R"(\[([^\]]+)\]\([^\)]+\))"), fmt});
    }

    // ── Images ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(198, 120, 221));   // purple
        m_rules.push_back({QRegularExpression(R"(!\[([^\]]*)\]\([^\)]+\))"), fmt});
    }

    // ── Blockquotes ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(130, 170, 130));
        fmt.setFontItalic(true);
        m_rules.push_back({QRegularExpression(R"(^> .+$)"), fmt});
    }

    // ── Unordered lists ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(224, 108, 117));
        m_rules.push_back({QRegularExpression(R"(^\s*[-*+] )"), fmt});
    }

    // ── Ordered lists ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(224, 108, 117));
        m_rules.push_back({QRegularExpression(R"(^\s*\d+\. )"), fmt});
    }

    // ── Task lists ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(86, 182, 194));
        m_rules.push_back({QRegularExpression(R"(^\s*[-*+] \[[ xX]\] )"), fmt});
    }

    // ── Horizontal rule ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(90, 90, 90));
        m_rules.push_back({QRegularExpression(R"(^(---|\*\*\*|___)\s*$)"), fmt});
    }

    // ── HTML tags ──
    {
        QTextCharFormat fmt;
        fmt.setForeground(QColor(150, 150, 150));
        m_rules.push_back({QRegularExpression(R"(<[^>]+>)"), fmt});
    }

    // ── Fenced code block markers ──
    m_codeBlockStart = QRegularExpression(R"(^```\w*\s*$)");
    m_codeBlockEnd = QRegularExpression(R"(^```\s*$)");

    m_codeBlockFormat.setForeground(QColor(190, 190, 190));
    m_codeBlockFormat.setBackground(QColor(40, 44, 52));
    m_codeBlockFormat.setFontFamilies({"JetBrains Mono"});
}

void MarkdownHighlighter::highlightBlock(const QString &text)
{
    // Handle fenced code blocks with state
    int prevState = previousBlockState();

    // Check if we're inside a code block
    if (prevState == 1) {
        // We're inside a code block
        setFormat(0, text.length(), m_codeBlockFormat);

        auto match = m_codeBlockEnd.match(text);
        if (match.hasMatch() && text.trimmed() == "```") {
            setCurrentBlockState(0);
        } else {
            setCurrentBlockState(1);
        }
        return;
    }

    // Check if this line starts a code block
    auto startMatch = m_codeBlockStart.match(text);
    if (startMatch.hasMatch()) {
        setFormat(0, text.length(), m_codeBlockFormat);
        setCurrentBlockState(1);
        return;
    }

    setCurrentBlockState(0);

    // Apply inline rules
    for (const auto &rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

} // namespace NanoMark
