/**
 * NanoMark — Annotations.cpp
 * Text highlighting, comments, bookmarks
 */

#include "Annotations.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>

namespace NanoMark {

QJsonObject Annotation::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["documentPath"] = documentPath;
    obj["startLine"] = startLine;
    obj["endLine"] = endLine;
    obj["startCol"] = startCol;
    obj["endCol"] = endCol;
    obj["type"] = type;
    obj["content"] = content;
    obj["color"] = color.name(QColor::HexArgb);
    obj["createdAt"] = createdAt;
    return obj;
}

Annotation Annotation::fromJson(const QJsonObject &obj)
{
    Annotation ann;
    ann.id = obj["id"].toString();
    ann.documentPath = obj["documentPath"].toString();
    ann.startLine = obj["startLine"].toInt();
    ann.endLine = obj["endLine"].toInt();
    ann.startCol = obj["startCol"].toInt();
    ann.endCol = obj["endCol"].toInt();
    ann.type = obj["type"].toString();
    ann.content = obj["content"].toString();
    ann.color = QColor(obj["color"].toString("#50ffeb3b"));
    ann.createdAt = obj["createdAt"].toString();
    return ann;
}

Annotations::Annotations(QObject *parent)
    : QObject(parent)
{
}

void Annotations::addAnnotation(const Annotation &ann)
{
    Annotation a = ann;
    if (a.id.isEmpty()) {
        a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    if (a.createdAt.isEmpty()) {
        a.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    }

    m_annotations.append(a);
    emit annotationAdded(a);
}

void Annotations::removeAnnotation(const QString &id)
{
    m_annotations.erase(
        std::remove_if(m_annotations.begin(), m_annotations.end(),
                        [&id](const Annotation &a) { return a.id == id; }),
        m_annotations.end());
    emit annotationRemoved(id);
}

QVector<Annotation> Annotations::annotationsForDocument(const QString &documentPath) const
{
    QVector<Annotation> result;
    for (const auto &ann : m_annotations) {
        if (ann.documentPath == documentPath) {
            result.append(ann);
        }
    }
    return result;
}

QVector<Annotation> Annotations::bookmarks() const
{
    QVector<Annotation> result;
    for (const auto &ann : m_annotations) {
        if (ann.type == "bookmark") {
            result.append(ann);
        }
    }
    return result;
}

void Annotations::save(const QString &filePath) const
{
    QJsonArray arr;
    for (const auto &ann : m_annotations) {
        arr.append(ann.toJson());
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void Annotations::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    m_annotations.clear();
    QJsonArray arr = doc.array();
    for (const auto &val : arr) {
        m_annotations.append(Annotation::fromJson(val.toObject()));
    }
}

} // namespace NanoMark
