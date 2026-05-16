/**
 * NanoMark — StickyNotes.cpp
 * Sticky notes persistence and management
 */

#include "StickyNotes.h"
#include "Logger.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>

namespace NanoMark {

// ─── StickyNote serialization ──────────────────────────────────────────────────

QJsonObject StickyNote::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["documentPath"] = documentPath;
    obj["lineNumber"] = lineNumber;
    obj["content"] = content;
    obj["color"] = color.name();
    obj["pinned"] = pinned;
    obj["createdAt"] = createdAt;
    obj["updatedAt"] = updatedAt;
    return obj;
}

StickyNote StickyNote::fromJson(const QJsonObject &obj)
{
    StickyNote note;
    note.id = obj["id"].toString();
    note.documentPath = obj["documentPath"].toString();
    note.lineNumber = obj["lineNumber"].toInt();
    note.content = obj["content"].toString();
    note.color = QColor(obj["color"].toString("#fff59d"));
    note.pinned = obj["pinned"].toBool();
    note.createdAt = obj["createdAt"].toString();
    note.updatedAt = obj["updatedAt"].toString();
    return note;
}

// ─── StickyNotes Manager ───────────────────────────────────────────────────────

StickyNotes::StickyNotes(QObject *parent)
    : QObject(parent)
{
}

void StickyNotes::addNote(const StickyNote &note)
{
    StickyNote n = note;
    if (n.id.isEmpty()) {
        n.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    if (n.createdAt.isEmpty()) {
        n.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    n.updatedAt = n.createdAt;

    m_notes.append(n);
    emit noteAdded(n);
}

void StickyNotes::removeNote(const QString &id)
{
    m_notes.erase(
        std::remove_if(m_notes.begin(), m_notes.end(),
                        [&id](const StickyNote &n) { return n.id == id; }),
        m_notes.end());
    emit noteRemoved(id);
}

void StickyNotes::updateNote(const StickyNote &note)
{
    for (auto &n : m_notes) {
        if (n.id == note.id) {
            n = note;
            n.updatedAt = QDateTime::currentDateTime().toString(Qt::ISODate);
            emit noteUpdated(n);
            return;
        }
    }
}

QVector<StickyNote> StickyNotes::notesForDocument(const QString &documentPath) const
{
    QVector<StickyNote> result;
    for (const auto &note : m_notes) {
        if (note.documentPath == documentPath) {
            result.append(note);
        }
    }
    return result;
}

void StickyNotes::save(const QString &filePath) const
{
    QJsonArray arr;
    for (const auto &note : m_notes) {
        arr.append(note.toJson());
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void StickyNotes::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    m_notes.clear();
    QJsonArray arr = doc.array();
    for (const auto &val : arr) {
        m_notes.append(StickyNote::fromJson(val.toObject()));
    }
}

} // namespace NanoMark
