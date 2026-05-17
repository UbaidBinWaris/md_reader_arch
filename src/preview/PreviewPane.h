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

    /** Set HTML content for display */
    void setHtml(const QString &html);

    /** Export current content to PDF */
    void exportToPDF(const QString &filePath, const QString &html);

private:
    void initWebEngine();

    QStackedLayout *m_layout = nullptr;
    QLabel *m_placeholder = nullptr;
    QWebEngineView *m_webView = nullptr;
    QString m_lastHtml;
};

} // namespace NanoMark
