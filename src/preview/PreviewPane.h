#pragma once

/**
 * NanoMark — PreviewPane.h
 * WebEngine-based live preview for rendered markdown
 */

#include <QWidget>
#include <QString>

class QWebEngineView;
class QStackedLayout;
class QLabel;

namespace NanoMark {

class PreviewPane : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewPane(QWidget *parent = nullptr);
    ~PreviewPane() override;

    /** Set HTML content for display (Legacy/Fallback) */
    void setHtml(const QString &html);

    /** Update preview using fast DOM innerHTML hot-swap */
    void updatePreview(const QString &htmlBody, bool isDark, const QString &css);

    /** Export current content to PDF */
    void exportToPDF(const QString &filePath, const QString &html);

private slots:
    void onLoadFinished(bool ok);
    void onLoadStarted();
    void onLoadProgress(int progress);

private:
    enum class PreviewState {
        Placeholder,
        Initializing,
        Ready,
        Failed
    };

    void initWebEngine();

    PreviewState m_state = PreviewState::Placeholder;
    QStackedLayout *m_layout = nullptr;
    QLabel *m_placeholder = nullptr;
    QWebEngineView *m_webView = nullptr;
    QString m_lastHtml;
    QString m_pendingHtml;

    // Fast innerHTML updates state
    bool m_shellLoaded = false;
    QString m_lastHtmlBody;
    struct PendingUpdate {
        QString htmlBody;
        bool isDark;
        QString css;
    } m_pendingUpdate;
    bool m_hasPendingUpdate = false;
};

} // namespace NanoMark
