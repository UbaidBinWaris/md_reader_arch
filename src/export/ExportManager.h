#pragma once

/**
 * NanoMark — ExportManager.h
 * Export orchestration
 */

#include <QObject>
#include <QString>

namespace NanoMark {

class ExportManager : public QObject
{
    Q_OBJECT

public:
    explicit ExportManager(QObject *parent = nullptr);

    /** Export markdown to PDF */
    bool exportToPDF(const QString &markdown, const QString &outputPath);

    /** Export markdown to standalone HTML */
    bool exportToHTML(const QString &markdown, const QString &outputPath);

signals:
    void exportCompleted(const QString &format, const QString &path);
    void exportFailed(const QString &format, const QString &error);
};

} // namespace NanoMark
