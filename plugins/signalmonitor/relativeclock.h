/*
  relativeclock.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_RELATIVECLOCK_H
#define GAMMARAY_RELATIVECLOCK_H

#include <QtGlobal>

namespace GammaRay {
class RelativeClock
{
public:
    explicit RelativeClock(qint64 offset = currentMSecsSinceEpoch())
        : m_offset(offset)
    {
    }

    qint64 mSecs() const
    {
        return currentMSecsSinceEpoch() - offset();
    }
    qint64 mSecs(qint64 alignment) const;

    qint64 offset() const
    {
        return m_offset;
    }
    static const RelativeClock *sinceAppStart();

private:
    static qint64 currentMSecsSinceEpoch();

private:
    const qint64 m_offset;
};

inline qint64 RelativeClock::mSecs(qint64 alignment) const
{
    const qint64 t = mSecs();
    return t - t % alignment;
}
} // namespace GammaRay

#endif // GAMMARAY_RELATIVECLOCK_H
