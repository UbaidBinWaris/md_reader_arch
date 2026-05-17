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
#include <QJsonDocument>
#include <QJsonArray>
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
    if (m_state != PreviewState::Placeholder) return;

    m_state = PreviewState::Initializing;
    Logger::instance().info("Deferred WebEngine initialization starting (Persistent HTML Shell)...");

    m_webView = new QWebEngineView(this);
    
    // Configure WebEngine settings
    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    // Hardening paint properties to prevent white flashes
    m_webView->setAttribute(Qt::WA_OpaquePaintEvent, false);
    m_webView->setStyleSheet("background: #0d0d0d;");
    m_webView->page()->setBackgroundColor(QColor("#0d0d0d"));
    
    // Connect lifecycle signals
    connect(m_webView, &QWebEngineView::loadStarted, this, &PreviewPane::onLoadStarted);
    connect(m_webView, &QWebEngineView::loadProgress, this, &PreviewPane::onLoadProgress);
    connect(m_webView, &QWebEngineView::loadFinished, this, &PreviewPane::onLoadFinished);
    
    m_layout->addWidget(m_webView);
    
    // Bootstrap the HTML shell ONCE
    QString shellHtml = R"(
    <!DOCTYPE html>
    <html>
    <head>
    <meta charset="utf-8">
    <style id="preview-style">
    html, body {
        background-color: #0d0d0d !important;
        color: #e3e3e3;
        font-family: Inter, system-ui, sans-serif;
        margin: 20px;
        line-height: 1.75;
    }
    </style>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/atom-one-dark.min.css" id="hljs-style">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"></script>
    <script>
    var lastScrollPercent = 0;
    
    // Track scroll position in shell
    window.addEventListener('scroll', function() {
        var doc = document.documentElement;
        var total = doc.scrollHeight - doc.clientHeight;
        if (total > 0) {
            lastScrollPercent = doc.scrollTop / total;
        }
    });

    function updateContent(html, isDark, css) {
        document.body.className = isDark ? "dark" : "light";
        document.body.style.backgroundColor = isDark ? "#0d0d0d" : "#ffffff";
        document.body.style.color = isDark ? "#e3e3e3" : "#1a1a1a";
        
        var style = document.getElementById("preview-style");
        if (style && css) {
            style.innerHTML = css;
        }
        
        var link = document.getElementById("hljs-style");
        if (link) {
            link.href = "https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/" + 
                      (isDark ? "atom-one-dark" : "atom-one-light") + ".min.css";
        }
        
        var contentDiv = document.getElementById("content");
        contentDiv.innerHTML = html;
        
        // Re-run highlighting
        document.querySelectorAll('pre code').forEach((el) => {
            hljs.highlightElement(el);
        });

        // Restore scroll percentage after DOM update
        var doc = document.documentElement;
        var total = doc.scrollHeight - doc.clientHeight;
        if (total > 0) {
            doc.scrollTop = lastScrollPercent * total;
        }
    }
    </script>
    </head>
    <body class="dark" style="background-color: #0d0d0d;">
    <div id="content"></div>
    </body>
    </html>
    )";

    m_webView->setHtml(shellHtml, QUrl("about:blank"));
}

void PreviewPane::onLoadStarted()
{
    Logger::instance().info("PreviewPane: Load started");
}

void PreviewPane::onLoadProgress(int /*progress*/)
{
    // Can be used for debugging
}

void PreviewPane::onLoadFinished(bool ok)
{
    if (!ok) {
        Logger::instance().error("PreviewPane: Load failed");
        m_state = PreviewState::Failed;
        return;
    }

    if (m_state == PreviewState::Initializing) {
        Logger::instance().info("PreviewPane: Initialization complete. Persistent WebEngine Ready.");
        m_state = PreviewState::Ready;
        m_shellLoaded = true;
        m_layout->setCurrentWidget(m_webView); // Swap placeholder -> WebEngine
        
        // Execute pending update if any
        if (m_hasPendingUpdate) {
            updatePreview(m_pendingUpdate.htmlBody, m_pendingUpdate.isDark, m_pendingUpdate.css);
            m_hasPendingUpdate = false;
        }

        // Push legacy queued HTML now that we are ready
        if (!m_pendingHtml.isEmpty()) {
            setHtml(m_pendingHtml);
            m_pendingHtml.clear();
        }
    }
}

PreviewPane::~PreviewPane() = default;

void PreviewPane::setHtml(const QString &html)
{
    if (html == m_lastHtml) return;
    m_lastHtml = html;

    if (m_state == PreviewState::Placeholder) {
        m_pendingHtml = html;
        initWebEngine();
        return;
    }

    if (m_state == PreviewState::Initializing) {
        m_pendingHtml = html; // Update queued HTML
        return;
    }

    if (m_state == PreviewState::Failed) {
        return;
    }

    m_webView->setHtml(html, QUrl("about:blank"));
}

void PreviewPane::updatePreview(const QString &htmlBody, bool isDark, const QString &css)
{
    if (htmlBody == m_lastHtmlBody) return;
    m_lastHtmlBody = htmlBody;

    if (m_state == PreviewState::Placeholder) {
        m_pendingUpdate = {htmlBody, isDark, css};
        m_hasPendingUpdate = true;
        initWebEngine();
        return;
    }

    if (m_state == PreviewState::Initializing) {
        m_pendingUpdate = {htmlBody, isDark, css};
        m_hasPendingUpdate = true;
        return;
    }

    if (m_state == PreviewState::Failed) {
        return;
    }

    // Escape strings cleanly using QJsonDocument (avoids quote breakages)
    QString escapedHtml = QJsonDocument(QJsonArray{htmlBody}).toJson(QJsonDocument::Compact);
    escapedHtml = escapedHtml.mid(1, escapedHtml.length() - 2); // Chop [ and ]

    QString escapedCss = QJsonDocument(QJsonArray{css}).toJson(QJsonDocument::Compact);
    escapedCss = escapedCss.mid(1, escapedCss.length() - 2); // Chop [ and ]

    QString jsCall = QString("updateContent(%1, %2, %3);")
        .arg(escapedHtml)
        .arg(isDark ? "true" : "false")
        .arg(escapedCss);

    m_webView->page()->runJavaScript(jsCall);
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
