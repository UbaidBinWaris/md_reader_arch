#include <QtTest>
#include "MarkdownRenderer.h"

using namespace NanoMark;

class TestMarkdownRenderer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        m_renderer = new MarkdownRenderer();
    }

    void cleanupTestCase() {
        delete m_renderer;
    }

    void testHeadings() {
        QString md = "# Heading 1";
        QString html = m_renderer->render(md);
        QVERIFY(html.contains("<h1 id='heading-1'>Heading 1</h1>"));
    }

    void testBold() {
        QString md = "**Bold Text**";
        QString html = m_renderer->render(md);
        QVERIFY(html.contains("<strong>Bold Text</strong>"));
    }

    void testTables() {
        QString md = "| Col 1 | Col 2 |\n|-------|-------|\n| Val 1 | Val 2 |";
        QString html = m_renderer->render(md);
        QVERIFY(html.contains("<table>"));
        QVERIFY(html.contains("<td>Val 1</td>"));
    }

private:
    MarkdownRenderer *m_renderer;
};

QTEST_MAIN(TestMarkdownRenderer)
#include "test_renderer.moc"
