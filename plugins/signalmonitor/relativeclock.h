/*
  relativeclock.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    qint64 mSecs() const { return currentMSecsSinceEpoch() - offset(); }
    qint64 mSecs(qint64 alignment) const;

    qint64 offset() const { return m_offset; }
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
