#pragma once

/**
 * NanoMark — NanomarkFile.h
 * .nanomark sidecar file format for portable annotations
 */

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QVector>

#include "StickyNotes.h"
#include "Annotations.h"

namespace NanoMark {

struct ReadingProgress {
    double percentage = 0.0;       // 0.0 to 1.0
    int lastScrollPosition = 0;
    int lastLine = 0;
    QString lastOpened;            // ISO date string
};

class NanomarkFile : public QObject
{
    Q_OBJECT

public:
    explicit NanomarkFile(QObject *parent = nullptr);

    /** Load .nanomark file for given markdown file */
    bool load(const QString &mdFilePath);

    /** Save .nanomark file alongside the markdown file */
    bool save(const QString &mdFilePath) const;

    /** Check if .nanomark file exists for a given markdown file */
    static bool exists(const QString &mdFilePath);

    /** Get the .nanomark path for a given markdown file */
    static QString nanomarkPath(const QString &mdFilePath);

    /** Access data */
    StickyNotes* stickyNotes() { return &m_stickyNotes; }
    Annotations* annotations() { return &m_annotations; }
    ReadingProgress& readingProgress() { return m_progress; }

signals:
    void loaded(const QString &path);
    void saved(const QString &path);
    void loadFailed(const QString &error);

private:
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &obj);

    StickyNotes m_stickyNotes;
    Annotations m_annotations;
    ReadingProgress m_progress;
};

} // namespace NanoMark
