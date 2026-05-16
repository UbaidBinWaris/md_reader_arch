/**
 * NanoMark — StudyMode.cpp
 * Study mode implementation
 */

#include "StudyMode.h"

namespace NanoMark {

StudyMode::StudyMode(QObject *parent)
    : QObject(parent)
{
}

void StudyMode::setActive(bool active)
{
    if (m_active == active) return;
    m_active = active;
    emit activeChanged(m_active);
}

void StudyMode::setZoomLevel(int level)
{
    level = qBound(50, level, 300);
    if (m_zoomLevel == level) return;
    m_zoomLevel = level;
    emit zoomChanged(m_zoomLevel);
}

void StudyMode::setFocusMode(bool enabled)
{
    if (m_focusMode == enabled) return;
    m_focusMode = enabled;
    emit focusModeChanged(m_focusMode);
}

} // namespace NanoMark
