/*
  probeguard.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    static void setInsideProbe(bool inside);

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
