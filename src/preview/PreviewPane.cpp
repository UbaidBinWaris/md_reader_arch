/**
 * NanoMark — PreviewPane.cpp
 * WebEngine-based live preview implementation
 */

#include "PreviewPane.h"

#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QPageLayout>
#include <QPageSize>
#include <QMarginsF>
#include <QTimer>

namespace NanoMark {

PreviewPane::PreviewPane(QWidget *parent)
    : QWebEngineView(parent)
{
    // Configure WebEngine settings
    QWebEngineSettings *settings = this->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    // Set a neutral background
    setStyleSheet("background: #1e1e2e;");

    // Load initial empty page
    QWebEngineView::setHtml(
        "<html><body style='background:#1e1e2e;color:#cdd6f4;font-family:sans-serif;"
        "display:flex;align-items:center;justify-content:center;height:100vh;'>"
        "<p style='opacity:0.5;font-size:14px;'>Start typing to see preview...</p>"
        "</body></html>");
}

PreviewPane::~PreviewPane() = default;

void PreviewPane::setHtml(const QString &html)
{
    if (html == m_lastHtml) return;
    m_lastHtml = html;

    // Store current scroll position, set html, then restore scroll
    page()->runJavaScript("window.scrollY", [this, html](const QVariant &scrollPos) {
        int scroll = scrollPos.toInt();
        QWebEngineView::setHtml(html, QUrl("about:blank"));

        // Restore scroll position after content loads
        QTimer::singleShot(100, this, [this, scroll]() {
            page()->runJavaScript(QString("window.scrollTo(0, %1);").arg(scroll));
        });
    });
}

void PreviewPane::exportToPDF(const QString &filePath, const QString &html)
{
    // Create a temporary page for PDF export
    QWebEnginePage *exportPage = new QWebEnginePage(this);
    exportPage->setHtml(html, QUrl("about:blank"));

    // Wait for page to load, then print to PDF
    connect(exportPage, &QWebEnginePage::loadFinished, this, [exportPage, filePath](bool ok) {
        if (!ok) {
            delete exportPage;
            return;
        }

        QPageLayout layout(QPageSize(QPageSize::A4), QPageLayout::Portrait,
                          QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

        exportPage->printToPdf(filePath, layout);

        // Clean up after a delay
        QTimer::singleShot(5000, exportPage, &QObject::deleteLater);
    });
}

} // namespace NanoMark
