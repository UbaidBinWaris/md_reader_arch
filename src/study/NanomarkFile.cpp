/**
 * NanoMark — NanomarkFile.cpp
 * .nanomark sidecar file implementation
 */

#include "NanomarkFile.h"
#include "Logger.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

namespace NanoMark {

NanomarkFile::NanomarkFile(QObject *parent)
    : QObject(parent)
{
}

QString NanomarkFile::nanomarkPath(const QString &mdFilePath)
{
    QFileInfo fi(mdFilePath);
    return fi.absolutePath() + "/" + fi.completeBaseName() + ".nanomark";
}

bool NanomarkFile::exists(const QString &mdFilePath)
{
    return QFile::exists(nanomarkPath(mdFilePath));
}

bool NanomarkFile::load(const QString &mdFilePath)
{
    QString path = nanomarkPath(mdFilePath);
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        // No .nanomark file is normal — not an error
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        emit loadFailed("Invalid .nanomark file format");
        return false;
    }

    fromJson(doc.object());

    Logger::instance().info("Loaded .nanomark: " + path);
    emit loaded(path);
    return true;
}

bool NanomarkFile::save(const QString &mdFilePath) const
{
    QString path = nanomarkPath(mdFilePath);
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().error("Failed to save .nanomark: " + file.errorString());
        return false;
    }

    QJsonDocument doc(toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    Logger::instance().info("Saved .nanomark: " + path);
    return true;
}

QJsonObject NanomarkFile::toJson() const
{
    QJsonObject root;
    root["version"] = "1.0";
    root["format"] = "nanomark";

    // Sticky notes
    QJsonArray notesArr;
    for (const auto &note : m_stickyNotes.allNotes()) {
        notesArr.append(note.toJson());
    }
    root["sticky_notes"] = notesArr;

    // Annotations
    QJsonArray annArr;
    for (const auto &ann : m_annotations.allAnnotations()) {
        annArr.append(ann.toJson());
    }
    root["annotations"] = annArr;

    // Reading progress
    QJsonObject progressObj;
    progressObj["percentage"] = m_progress.percentage;
    progressObj["scroll_position"] = m_progress.lastScrollPosition;
    progressObj["last_line"] = m_progress.lastLine;
    progressObj["last_opened"] = m_progress.lastOpened;
    root["reading_progress"] = progressObj;

    return root;
}

void NanomarkFile::fromJson(const QJsonObject &obj)
{
    // Sticky notes
    QJsonArray notesArr = obj["sticky_notes"].toArray();
    for (const auto &val : notesArr) {
        m_stickyNotes.addNote(StickyNote::fromJson(val.toObject()));
    }

    // Annotations
    QJsonArray annArr = obj["annotations"].toArray();
    for (const auto &val : annArr) {
        m_annotations.addAnnotation(Annotation::fromJson(val.toObject()));
    }

    // Reading progress
    QJsonObject progressObj = obj["reading_progress"].toObject();
    m_progress.percentage = progressObj["percentage"].toDouble();
    m_progress.lastScrollPosition = progressObj["scroll_position"].toInt();
    m_progress.lastLine = progressObj["last_line"].toInt();
    m_progress.lastOpened = progressObj["last_opened"].toString();
}

} // namespace NanoMark
