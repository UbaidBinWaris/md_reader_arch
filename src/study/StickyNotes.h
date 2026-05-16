#pragma once

/**
 * NanoMark — StickyNotes.h
 * Persistent sticky notes attached to documents
 */

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>
#include <QJsonObject>

namespace NanoMark {

struct StickyNote {
    QString id;
    QString documentPath;
    int lineNumber = 0;
    QString content;
    QColor color = QColor(255, 245, 157);  // default yellow
    bool pinned = false;
    QString createdAt;
    QString updatedAt;

    QJsonObject toJson() const;
    static StickyNote fromJson(const QJsonObject &obj);
};

class StickyNotes : public QObject
{
    Q_OBJECT

public:
    explicit StickyNotes(QObject *parent = nullptr);

    /** Add a sticky note */
    void addNote(const StickyNote &note);

    /** Remove a sticky note by ID */
    void removeNote(const QString &id);

    /** Update a sticky note */
    void updateNote(const StickyNote &note);

    /** Get all notes for a document */
    QVector<StickyNote> notesForDocument(const QString &documentPath) const;

    /** Save notes to disk */
    void save(const QString &filePath) const;

    /** Load notes from disk */
    void load(const QString &filePath);

signals:
    void noteAdded(const StickyNote &note);
    void noteRemoved(const QString &id);
    void noteUpdated(const StickyNote &note);

private:
    QVector<StickyNote> m_notes;
};

} // namespace NanoMark
