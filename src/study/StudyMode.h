#pragma once

/**
 * NanoMark — StudyMode.h
 * Study/reading mode controller
 */

#include <QObject>

namespace NanoMark {

class StudyMode : public QObject
{
    Q_OBJECT

public:
    explicit StudyMode(QObject *parent = nullptr);

    bool isActive() const { return m_active; }
    void setActive(bool active);

    int zoomLevel() const { return m_zoomLevel; }
    void setZoomLevel(int level);

    bool focusMode() const { return m_focusMode; }
    void setFocusMode(bool enabled);

signals:
    void activeChanged(bool active);
    void zoomChanged(int level);
    void focusModeChanged(bool enabled);

private:
    bool m_active = false;
    int m_zoomLevel = 100;
    bool m_focusMode = false;
};

} // namespace NanoMark
