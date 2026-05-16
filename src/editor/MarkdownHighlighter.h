#pragma once

/**
 * NanoMark — MarkdownHighlighter.h
 * Syntax highlighter for markdown source editing
 */

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <vector>

namespace NanoMark {

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit MarkdownHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    std::vector<HighlightRule> m_rules;

    // Block-level state for fenced code blocks
    QTextCharFormat m_codeBlockFormat;
    QRegularExpression m_codeBlockStart;
    QRegularExpression m_codeBlockEnd;
};

} // namespace NanoMark
