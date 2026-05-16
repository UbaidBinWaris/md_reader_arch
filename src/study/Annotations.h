#pragma once

/**
 * NanoMark — Annotations.h
 * Text highlighting, comments, bookmarks
 */

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>
#include <QJsonObject>

namespace NanoMark {

struct Annotation {
    QString id;
    QString documentPath;
    int startLine = 0;
    int endLine = 0;
    int startCol = 0;
    int endCol = 0;
    QString type;  // "highlight", "comment", "bookmark"
    QString content;
    QColor color = QColor(255, 235, 59, 80);
    QString createdAt;

    QJsonObject toJson() const;
    static Annotation fromJson(const QJsonObject &obj);
};

class Annotations : public QObject
{
    Q_OBJECT

public:
    explicit Annotations(QObject *parent = nullptr);

    void addAnnotation(const Annotation &ann);
    void removeAnnotation(const QString &id);
    QVector<Annotation> annotationsForDocument(const QString &documentPath) const;
    QVector<Annotation> bookmarks() const;

    void save(const QString &filePath) const;
    void load(const QString &filePath);

signals:
    void annotationAdded(const Annotation &ann);
    void annotationRemoved(const QString &id);

private:
    QVector<Annotation> m_annotations;
};

} // namespace NanoMark
