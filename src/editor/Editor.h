#pragma once

/**
 * NanoMark — Editor.h
 * Custom markdown editor with line numbers, formatting helpers, find/replace
 */

#include <QPlainTextEdit>
#include <QWidget>

namespace NanoMark {

class MarkdownHighlighter;

class Editor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor() override;

    /** Insert prefix formatting at start of current line (e.g. "# ") */
    void insertFormatting(const QString &prefix, const QString &suffix);

    /** Wrap selected text with before/after markers (e.g. "**" / "**") */
    void wrapSelection(const QString &before, const QString &after);

    /** Insert text at cursor position */
    void insertAtCursor(const QString &text);

    /** Show the find/replace bar */
    void showFindReplaceBar();

    /** Line number area width */
    int lineNumberAreaWidth() const;

    /** HTML caching for fast tab switching */
    void setCachedHtml(const QString &html);
    QString cachedHtml() const;
    bool isHtmlDirty() const;
    void markHtmlClean();
    QTextBlock firstVisibleBlock() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    MarkdownHighlighter *m_highlighter = nullptr;
    QWidget *m_lineNumberArea = nullptr;
    QWidget *m_findReplaceBar = nullptr;

    QString m_cachedHtml;
    bool m_htmlDirty = true;
};

// ─── Line Number Area Widget ───────────────────────────────────────────────────

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(Editor *editor) : QWidget(editor), m_editor(editor) {}

    QSize sizeHint() const override {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        m_editor->lineNumberAreaPaintEvent(event);
    }

private:
    Editor *m_editor;
};

} // namespace NanoMark
