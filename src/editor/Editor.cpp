/**
 * NanoMark — Editor.cpp
 * Custom markdown editor implementation
 */

#include "Editor.h"
#include "MarkdownHighlighter.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRegularExpression>

namespace NanoMark {

Editor::Editor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    // Set up font
    QFont editorFont("JetBrains Mono", 13);
    editorFont.setStyleHint(QFont::Monospace);
    editorFont.setFixedPitch(true);
    setFont(editorFont);

    // Tab width = 4 spaces
    QFontMetrics fm(editorFont);
    setTabStopDistance(fm.horizontalAdvance(' ') * 4);

    // Line number area
    m_lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &Editor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &Editor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &Editor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // Syntax highlighter
    m_highlighter = new MarkdownHighlighter(document());

    // Editor settings
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    setCursorWidth(2);

    // Smooth scrolling
    verticalScrollBar()->setSingleStep(3);
}

Editor::~Editor() = default;

// ─── Formatting Helpers ────────────────────────────────────────────────────────

void Editor::insertFormatting(const QString &prefix, const QString &suffix)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        cursor.insertText(prefix + selected + suffix);
    } else {
        // Insert at beginning of current line
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.insertText(prefix);
        if (!suffix.isEmpty()) {
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.insertText(suffix);
        }
    }

    cursor.endEditBlock();
    setTextCursor(cursor);
}

void Editor::wrapSelection(const QString &before, const QString &after)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();

        cursor.setPosition(end);
        cursor.insertText(after);
        cursor.setPosition(start);
        cursor.insertText(before);

        // Re-select the wrapped text
        cursor.setPosition(start + before.length());
        cursor.setPosition(end + before.length(), QTextCursor::KeepAnchor);
    } else {
        // Insert markers and place cursor between them
        int pos = cursor.position();
        cursor.insertText(before + after);
        cursor.setPosition(pos + before.length());
    }

    cursor.endEditBlock();
    setTextCursor(cursor);
}

void Editor::insertAtCursor(const QString &text)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.insertText(text);
    cursor.endEditBlock();
    setTextCursor(cursor);
}

// ─── Find/Replace Bar ─────────────────────────────────────────────────────────

void Editor::showFindReplaceBar()
{
    if (m_findReplaceBar) {
        m_findReplaceBar->show();
        m_findReplaceBar->findChild<QLineEdit*>("findInput")->setFocus();
        return;
    }

    m_findReplaceBar = new QWidget(this);
    auto *layout = new QHBoxLayout(m_findReplaceBar);
    layout->setContentsMargins(8, 4, 8, 4);

    auto *findInput = new QLineEdit(m_findReplaceBar);
    findInput->setObjectName("findInput");
    findInput->setPlaceholderText("Find...");
    findInput->setMinimumWidth(200);

    auto *replaceInput = new QLineEdit(m_findReplaceBar);
    replaceInput->setPlaceholderText("Replace...");
    replaceInput->setMinimumWidth(200);

    auto *findBtn = new QPushButton("Find", m_findReplaceBar);
    auto *replaceBtn = new QPushButton("Replace", m_findReplaceBar);
    auto *replaceAllBtn = new QPushButton("Replace All", m_findReplaceBar);
    auto *closeBtn = new QPushButton("✕", m_findReplaceBar);
    closeBtn->setMaximumWidth(30);

    layout->addWidget(new QLabel("Find:"));
    layout->addWidget(findInput);
    layout->addWidget(findBtn);
    layout->addWidget(new QLabel("Replace:"));
    layout->addWidget(replaceInput);
    layout->addWidget(replaceBtn);
    layout->addWidget(replaceAllBtn);
    layout->addStretch();
    layout->addWidget(closeBtn);

    // Find action
    connect(findBtn, &QPushButton::clicked, this, [this, findInput]() {
        QString searchTerm = findInput->text();
        if (searchTerm.isEmpty()) return;
        find(searchTerm);
    });

    connect(findInput, &QLineEdit::returnPressed, findBtn, &QPushButton::click);

    // Replace action
    connect(replaceBtn, &QPushButton::clicked, this, [this, findInput, replaceInput]() {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection() && cursor.selectedText() == findInput->text()) {
            cursor.insertText(replaceInput->text());
            setTextCursor(cursor);
        }
        find(findInput->text());
    });

    // Replace All action
    connect(replaceAllBtn, &QPushButton::clicked, this, [this, findInput, replaceInput]() {
        QString content = toPlainText();
        content.replace(findInput->text(), replaceInput->text());
        setPlainText(content);
    });

    // Close
    connect(closeBtn, &QPushButton::clicked, m_findReplaceBar, &QWidget::hide);

    m_findReplaceBar->setGeometry(0, 0, width(), 40);
    m_findReplaceBar->show();
    findInput->setFocus();
}

// ─── Line Number Area ─────────────────────────────────────────────────────────

int Editor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    return 16 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void Editor::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void Editor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void Editor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                        lineNumberAreaWidth(), cr.height()));

    // Reposition find/replace bar if visible
    if (m_findReplaceBar && m_findReplaceBar->isVisible()) {
        m_findReplaceBar->setGeometry(0, 0, width(), 40);
    }
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(13, 13, 13));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    QFont numFont = font();
    int numSize = qMax(1, font().pointSize() - 1);
    numFont.setPointSize(numSize);
    painter.setFont(numFont);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber == textCursor().blockNumber()) {
                painter.setPen(QColor(160, 160, 160));
            } else {
                painter.setPen(QColor(80, 80, 80));
            }
            painter.drawText(0, top, m_lineNumberArea->width() - 6,
                           fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void Editor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(23, 23, 23));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

// ─── Key Handling ─────────────────────────────────────────────────────────────

void Editor::keyPressEvent(QKeyEvent *event)
{
    // Auto-indent on Enter
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString currentLine = cursor.block().text();

        // Get leading whitespace
        int indent = 0;
        for (QChar ch : currentLine) {
            if (ch == ' ' || ch == '\t') indent++;
            else break;
        }
        QString whitespace = currentLine.left(indent);

        // Auto-continue list items
        static QRegularExpression listPattern(R"(^(\s*)([-*+]|\d+\.) )");
        auto match = listPattern.match(currentLine);

        QPlainTextEdit::keyPressEvent(event);

        if (match.hasMatch()) {
            QString prefix = match.captured(0);
            // If the line was just the list marker (empty item), remove it
            if (currentLine.trimmed() == match.captured(2)) {
                cursor = textCursor();
                cursor.movePosition(QTextCursor::PreviousBlock);
                cursor.select(QTextCursor::BlockUnderCursor);
                cursor.removeSelectedText();
                cursor.deletePreviousChar(); // remove the newline
                setTextCursor(cursor);
            } else {
                // Continue with the same list marker
                insertPlainText(prefix);
            }
        } else if (!whitespace.isEmpty()) {
            insertPlainText(whitespace);
        }
        return;
    }

    // Tab key: insert spaces
    if (event->key() == Qt::Key_Tab) {
        if (textCursor().hasSelection()) {
            // Indent selected lines
            QTextCursor cursor = textCursor();
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();

            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

            QString selected = cursor.selectedText();
            // QTextCursor uses \u2029 as paragraph separator
            selected.replace(QChar(0x2029), "\n    ");
            selected.prepend("    ");

            cursor.insertText(selected);
        } else {
            insertPlainText("    ");
        }
        return;
    }

    // Escape to close find bar
    if (event->key() == Qt::Key_Escape && m_findReplaceBar && m_findReplaceBar->isVisible()) {
        m_findReplaceBar->hide();
        return;
    }

    QPlainTextEdit::keyPressEvent(event);
}

} // namespace NanoMark
