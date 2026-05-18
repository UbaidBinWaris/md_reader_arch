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
#include <QTextBrowser>
#include <QJsonDocument>
#include <QJsonArray>
#include "Logger.h"
#include "MainWindow.h"
#include "Editor.h"
#include <QApplication>
#include <QWheelEvent>
#include <QScrollBar>

namespace NanoMark {

static void installScrollFilterRecursive(QObject *obj, QObject *filter) {
    if (!obj) return;
    obj->installEventFilter(filter);
    for (QObject *child : obj->children()) {
        installScrollFilterRecursive(child, filter);
    }
}

PreviewPane::PreviewPane(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#0d0d0d"));
    setPalette(pal);

    m_layout = new QStackedLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    // Create lightweight placeholder
    m_placeholder = new QLabel("Initializing Preview...", this);
    m_placeholder->setAttribute(Qt::WA_OpaquePaintEvent, true);
    m_placeholder->setAutoFillBackground(true);
    m_placeholder->setStyleSheet(
        "background: #0d0d0d; color: #555555; font-family: Inter, sans-serif; "
        "font-size: 14px; letter-spacing: 0.02em;"
    );
    m_placeholder->setAlignment(Qt::AlignCenter);

    m_layout->addWidget(m_placeholder);
    m_layout->setCurrentWidget(m_placeholder);

    m_statusOverlay = new QLabel(this);
    m_statusOverlay->setAlignment(Qt::AlignCenter);
    m_statusOverlay->setStyleSheet(
        "background-color: rgba(20, 20, 20, 0.9); color: #888888; "
        "border: 1px solid #2a2a2a; border-radius: 4px; padding: 4px 8px; "
        "font-family: Inter, sans-serif; font-size: 11px; font-weight: 600;"
    );
    m_statusOverlay->setText("Initializing...");
    m_statusOverlay->hide();
}

void PreviewPane::initWebEngine()
{
    if (m_state != PreviewState::Placeholder) return;

    m_state = PreviewState::Initializing;
    updateStatusOverlay("Preview Initializing...", "#a0a0a0");
    Logger::instance().info("Deferred WebEngine initialization starting (Persistent HTML Shell)...");

    m_webView = new QWebEngineView(this);
    
    // Configure WebEngine settings
    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    // Hardening paint properties to prevent white flashes & Wayland transparency leakage
    m_webView->setAttribute(Qt::WA_OpaquePaintEvent, true);
    m_webView->setAutoFillBackground(true);
    QPalette webPal = m_webView->palette();
    webPal.setColor(QPalette::Base, QColor("#0d0d0d"));
    webPal.setColor(QPalette::Window, QColor("#0d0d0d"));
    m_webView->setPalette(webPal);
    m_webView->setStyleSheet("background: #0d0d0d;");
    m_webView->page()->setBackgroundColor(QColor("#0d0d0d"));
    
    // Connect lifecycle signals
    connect(m_webView, &QWebEngineView::loadStarted, this, &PreviewPane::onLoadStarted);
    connect(m_webView, &QWebEngineView::loadProgress, this, &PreviewPane::onLoadProgress);
    connect(m_webView, &QWebEngineView::loadFinished, this, &PreviewPane::onLoadFinished);
    connect(m_webView->page(), &QWebEnginePage::scrollPositionChanged, this, [this](const QPointF &) {
        m_webView->page()->runJavaScript("window.getActiveHeadingLineNumber ? window.getActiveHeadingLineNumber() : -1", [this](const QVariant &res) {
            if (res.isValid()) {
                int line = res.toInt();
                if (line > 0) {
                    emit headingVisibleAtTopChanged(line);
                }
            }
        });
    });
    
    m_layout->addWidget(m_webView);
    installScrollFilterRecursive(m_webView, this);
    
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
    /* Hide scrollbars completely to achieve unified scroll layouts */
    ::-webkit-scrollbar {
        display: none;
    }
    html {
        scrollbar-width: none;
        -ms-overflow-style: none;
    }
    </style>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/styles/atom-one-dark.min.css" id="hljs-style">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"></script>
    <script>
    // Global error tracker
    window.onerror = function(msg, url, line) {
        console.error("JS ERROR: " + msg + " at " + url + ":" + line);
        return false;
    };

    var lastScrollPercent = 0;
    
    // Track scroll position in shell
    window.addEventListener('scroll', function() {
        var doc = document.documentElement;
        var total = doc.scrollHeight - doc.clientHeight;
        if (total > 0) {
            lastScrollPercent = doc.scrollTop / total;
        }
    });

    window.scrollToPercentage = function(percent) {
        var doc = document.documentElement;
        var total = doc.scrollHeight - doc.clientHeight;
        if (total > 0) {
            doc.scrollTop = percent * total;
            lastScrollPercent = percent;
        }
    };

    window.getActiveHeadingLineNumber = function() {
        var headings = document.querySelectorAll('h1, h2, h3, h4, h5, h6');
        var activeLine = -1;
        for (var i = 0; i < headings.length; i++) {
            var h = headings[i];
            var rect = h.getBoundingClientRect();
            if (rect.top <= 120) {
                var line = parseInt(h.getAttribute('data-line'));
                if (!isNaN(line)) {
                    activeLine = line;
                }
            }
        }
        return activeLine;
    };

    window.updateContent = function(html, isDark, css) {
        // Validation check
        var contentDiv = document.getElementById("content");
        if (!contentDiv) {
            console.error("DOM not ready: #content div not found!");
            return false;
        }

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
        
        contentDiv.innerHTML = html;
        
        // Re-run highlighting if highlight.js loaded successfully
        if (typeof hljs !== 'undefined') {
            document.querySelectorAll('pre code').forEach((el) => {
                hljs.highlightElement(el);
            });
        }

        // Restore scroll percentage after DOM update
        var doc = document.documentElement;
        var total = doc.scrollHeight - doc.clientHeight;
        if (total > 0) {
            doc.scrollTop = lastScrollPercent * total;
        }
        return true;
    }

    // Set handshake ready
    window.nanoMarkReady = true;
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
    m_handshakeRetries = 0;
    updateStatusOverlay("Preview Initializing...", "#a0a0a0");
    Logger::instance().info("PreviewPane: Load started");
}

void PreviewPane::onLoadProgress(int progress)
{
    if (m_state == PreviewState::Initializing) {
        updateStatusOverlay(QString("Preview Initializing... %1%").arg(progress), "#10a37f");
    }
}

void PreviewPane::onLoadFinished(bool ok)
{
    if (!ok) {
        Logger::instance().error("PreviewPane: Load failed. Falling back to QTextBrowser...");
        updateStatusOverlay("Preview Failed", "#ef4444");
        switchToFallback();
        return;
    }

    if (m_state == PreviewState::Initializing) {
        // Timing handshake: ensure JavaScript bootstrap has completed execution
        m_webView->page()->runJavaScript("window.nanoMarkReady === true", [this](const QVariant &ready) {
            if (ready.toBool()) {
                Logger::instance().info("PreviewPane: Handshake successful. Persistent WebEngine Ready.");
                m_state = PreviewState::Ready;
                m_shellLoaded = true;
                updateStatusOverlay("Preview Ready", "#10a37f");
                m_layout->setCurrentWidget(m_webView); // Swap placeholder -> WebEngine
                
                // Execute pending update if any
                if (m_hasPendingUpdate) {
                    m_lastHtmlBody.clear(); // Force injection on handshake success
                    updatePreview(m_pendingUpdate.htmlBody, m_pendingUpdate.isDark, m_pendingUpdate.css);
                    m_hasPendingUpdate = false;
                }

                // Push legacy queued HTML now that we are ready
                if (!m_pendingHtml.isEmpty()) {
                    m_lastHtml.clear(); // Force injection on handshake success
                    setHtml(m_pendingHtml);
                    m_pendingHtml.clear();
                }
            } else {
                m_handshakeRetries++;
                if (m_handshakeRetries > 200) { // 2 seconds of 10ms retries
                    Logger::instance().error("PreviewPane: Handshake timed out after 2 seconds. Falling back to QTextBrowser...");
                    updateStatusOverlay("Preview Failed", "#ef4444");
                    switchToFallback();
                } else {
                    Logger::instance().warning(QString("PreviewPane: Handshake not ready yet (retry %1). Retrying in 10ms...").arg(m_handshakeRetries));
                    QTimer::singleShot(10, this, [this]() { onLoadFinished(true); });
                }
            }
        });
    }
}

PreviewPane::~PreviewPane() = default;

void PreviewPane::setHtml(const QString &html)
{
    if (m_state == PreviewState::Ready && html == m_lastHtml) return;
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
        if (!m_fallbackBrowser) {
            m_fallbackBrowser = new QTextBrowser(this);
            m_fallbackBrowser->setAttribute(Qt::WA_OpaquePaintEvent, true);
            m_fallbackBrowser->setAutoFillBackground(true);
            m_fallbackBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_fallbackBrowser->installEventFilter(this);
            m_fallbackBrowser->setStyleSheet("background: #0d0d0d; color: #e3e3e3; border: none; font-family: Inter, sans-serif; font-size: 14px; padding: 20px;");
            m_layout->addWidget(m_fallbackBrowser);
            m_layout->setCurrentWidget(m_fallbackBrowser);
        }
        m_fallbackBrowser->setHtml(html);
        return;
    }

    m_webView->setHtml(html, QUrl("about:blank"));
}

void PreviewPane::updatePreview(const QString &htmlBody, bool isDark, const QString &css)
{
    if (m_state == PreviewState::Ready && htmlBody == m_lastHtmlBody) return;
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
        if (!m_fallbackBrowser) {
            m_fallbackBrowser = new QTextBrowser(this);
            m_fallbackBrowser->setAttribute(Qt::WA_OpaquePaintEvent, true);
            m_fallbackBrowser->setAutoFillBackground(true);
            m_fallbackBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_fallbackBrowser->installEventFilter(this);
            m_fallbackBrowser->setStyleSheet(isDark ? 
                "background: #0d0d0d; color: #e3e3e3; border: none; font-family: Inter, sans-serif; font-size: 14px; padding: 20px;" :
                "background: #ffffff; color: #1a1a1a; border: none; font-family: Inter, sans-serif; font-size: 14px; padding: 20px;");
            m_layout->addWidget(m_fallbackBrowser);
            m_layout->setCurrentWidget(m_fallbackBrowser);
        }
        m_fallbackBrowser->setHtml(QString("<html><head><style>%1</style></head><body class='%2'>%3</body></html>")
            .arg(css, isDark ? "dark" : "light", htmlBody));
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

    m_webView->page()->runJavaScript(jsCall, [](const QVariant &res) {
        if (res.toBool()) {
            Logger::instance().info("PreviewPane: DOM innerHTML swap executed successfully.");
        } else {
            Logger::instance().warning("PreviewPane: DOM innerHTML swap returned false or failed.");
        }
    });
}

void PreviewPane::scrollToPercentage(double percentage)
{
    if (m_state != PreviewState::Ready) return;
    QString jsCall = QString("scrollToPercentage(%1);").arg(percentage);
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

bool PreviewPane::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        QWidget *p = parentWidget();
        while (p) {
            if (MainWindow *mw = qobject_cast<MainWindow*>(p)) {
                if (auto *editor = mw->currentEditor()) {
                    if (editor->isVisible()) {
                        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                        QApplication::sendEvent(editor->verticalScrollBar(), wheelEvent);
                        return true;
                    }
                }
                break;
            }
            p = p->parentWidget();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void PreviewPane::switchToFallback()
{
    m_state = PreviewState::Failed;
    if (!m_fallbackBrowser) {
        m_fallbackBrowser = new QTextBrowser(this);
        m_fallbackBrowser->setAttribute(Qt::WA_OpaquePaintEvent, true);
        m_fallbackBrowser->setAutoFillBackground(true);
        m_fallbackBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_fallbackBrowser->installEventFilter(this);
        m_fallbackBrowser->setStyleSheet("background: #0d0d0d; color: #e3e3e3; border: none; font-family: Inter, sans-serif; font-size: 14px; padding: 20px;");
        m_layout->addWidget(m_fallbackBrowser);
    }
    m_layout->setCurrentWidget(m_fallbackBrowser);
    updateStatusOverlay("Fallback Active", "#eab308");
    
    if (m_hasPendingUpdate) {
        m_fallbackBrowser->setHtml(QString("<html><head><style>%1</style></head><body class='%2'>%3</body></html>")
            .arg(m_pendingUpdate.css, m_pendingUpdate.isDark ? "dark" : "light", m_pendingUpdate.htmlBody));
    } else if (!m_pendingHtml.isEmpty()) {
        m_fallbackBrowser->setHtml(m_pendingHtml);
    }
}

void PreviewPane::updateStatusOverlay(const QString &text, const QString &color)
{
    if (!m_statusOverlay) return;
    m_statusOverlay->setText(text);
    m_statusOverlay->setStyleSheet(
        QString("background-color: rgba(20, 20, 20, 0.9); color: %1; "
                "border: 1px solid #2a2a2a; border-radius: 4px; padding: 4px 8px; "
                "font-family: Inter, sans-serif; font-size: 11px; font-weight: 600;")
        .arg(color)
    );
    m_statusOverlay->adjustSize();
    
    // Reposition floating badge
    int w = m_statusOverlay->width();
    m_statusOverlay->move(width() - w - 16, 16);
    m_statusOverlay->show();

    // Auto-hide when loading is successful or fallback is active
    if (text == "Preview Ready" || text == "Fallback Active") {
        QTimer::singleShot(2000, m_statusOverlay, &QWidget::hide);
    }
}

void PreviewPane::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_statusOverlay) {
        int w = m_statusOverlay->width();
        m_statusOverlay->move(width() - w - 16, 16);
    }
}

} // namespace NanoMark
