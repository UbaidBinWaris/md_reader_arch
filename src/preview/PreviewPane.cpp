/**
 * NanoMark — PreviewPane.cpp
 * WebEngine-based live preview implementation
 */

#include "PreviewPane.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QPageLayout>
#include <QPageSize>
#include <QMarginsF>
#include <QTimer>
#include <QStackedLayout>
#include <QLabel>
#include "Logger.h"

namespace NanoMark {

PreviewPane::PreviewPane(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QStackedLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    // Create lightweight placeholder
    m_placeholder = new QLabel("Initializing Preview...", this);
    m_placeholder->setStyleSheet(
        "background: #0d0d0d; color: #555555; font-family: Inter, sans-serif; "
        "font-size: 14px; letter-spacing: 0.02em;"
    );
    m_placeholder->setAlignment(Qt::AlignCenter);

    m_layout->addWidget(m_placeholder);
    m_layout->setCurrentWidget(m_placeholder);
}

void PreviewPane::initWebEngine()
{
    if (m_webView) return;

    Logger::instance().info("Deferred WebEngine initialization starting...");

    m_webView = new QWebEngineView(this);
    
    // Configure WebEngine settings
    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    m_webView->setStyleSheet("background: #0d0d0d;");
    
    m_layout->addWidget(m_webView);
    m_layout->setCurrentWidget(m_webView);

    Logger::instance().info("WebEngine initialized successfully.");
}

PreviewPane::~PreviewPane() = default;

void PreviewPane::setHtml(const QString &html)
{
    if (html == m_lastHtml) return;
    m_lastHtml = html;

    initWebEngine();

    // Store current scroll position, set html, then restore scroll
    m_webView->page()->runJavaScript("window.scrollY", [this, html](const QVariant &scrollPos) {
        int scroll = scrollPos.toInt();
        m_webView->setHtml(html, QUrl("about:blank"));

        // Restore scroll position after content loads
        QTimer::singleShot(100, this, [this, scroll]() {
            m_webView->page()->runJavaScript(QString("window.scrollTo(0, %1);").arg(scroll));
        });
    });
}

void PreviewPane::exportToPDF(const QString &filePath, const QString &html)
{
    initWebEngine();

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
