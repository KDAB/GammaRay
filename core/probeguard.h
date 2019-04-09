/*
  probeguard.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_PROBEGUARD_H
#define GAMMARAY_PROBEGUARD_H

#include "gammaray_core_export.h"

#include <qglobal.h>

namespace GammaRay {
/** Use this inside probe code that might trigger expensive or otherwise problematic side-effects in
 * GammaRay itself, so the triggered probe part can skip reacting to that action.
 *
 * This works per-thread, and is supposed to be used RAII-style.
 *
 * Example: Creating a short-lived QObject inside a probe plugin.
 */
class GAMMARAY_CORE_EXPORT ProbeGuard
{
public:
    ProbeGuard();
    ~ProbeGuard();

    /** Use this inside your probe code to check if you have been called from other probe code.
     *  In that case you might want to skip some operations,
     */
    static bool insideProbe();
protected:
    ProbeGuard(bool newState);
    void setInsideProbe(bool inside);
private:
    Q_DISABLE_COPY(ProbeGuard)
    bool m_previousState;
};

/** This is the inverse of ProbeGuard, use this to temporarily disable the guard when doing
 *  individual calls to target code that might create objects, while being in a code block
 *  protected by ProbeGuard.
 */
class ProbeGuardSuspender : public ProbeGuard
{
public:
    ProbeGuardSuspender();
    ~ProbeGuardSuspender();
};
}

#endif // GAMMARAY_PROBEGUARD_H
