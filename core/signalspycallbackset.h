/*
  signalspycallbackset.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
