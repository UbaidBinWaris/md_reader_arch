#pragma once

/**
 * NanoMark — PreviewPane.h
 * WebEngine-based live preview for rendered markdown
 */

#include <QWebEngineView>
#include <QString>

namespace NanoMark {

class PreviewPane : public QWebEngineView
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
    QString m_lastHtml;
};

} // namespace NanoMark
