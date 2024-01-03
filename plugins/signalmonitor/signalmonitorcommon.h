/*
  signalmonitorcommon.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALMONITORCOMMON_H
#define GAMMARAY_SIGNALMONITORCOMMON_H

#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QVector>

namespace GammaRay {
namespace StreamOperators {
void registerSignalMonitorStreamOperators();
}
}

#endif // GAMMARAY_SIGNALMONITORCOMMON_H
