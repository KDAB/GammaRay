/*
  signalspycallbackset.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SIGNALSPYCALLBACKSET_H
#define GAMMARAY_SIGNALSPYCALLBACKSET_H

#include "gammaray_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

namespace GammaRay {
/** @brief Callbacks for tracing signal emissions and slot invocation.
 *
 *  @since 2.3
 */
struct GAMMARAY_CORE_EXPORT SignalSpyCallbackSet
{
    SignalSpyCallbackSet() = default;
    bool isNull() const;

    using BeginCallback = void (*)(QObject *, int, void **);
    using EndCallback = void (*)(QObject *, int);

    BeginCallback signalBeginCallback = nullptr;
    EndCallback signalEndCallback = nullptr;
    BeginCallback slotBeginCallback = nullptr;
    EndCallback slotEndCallback = nullptr;
};
}

#endif
