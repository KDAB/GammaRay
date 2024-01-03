/*
  probesettings.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROBESETTINGS_H
#define GAMMARAY_PROBESETTINGS_H

#include "gammaray_core_export.h"

#include <QVariant>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

namespace GammaRay {
/** General purpose settings of the probe provided by the launcher. */
namespace ProbeSettings {
GAMMARAY_CORE_EXPORT QVariant value(const QString &key, const QVariant &defaultValue = QString());

/** Call if using runtime attaching to obtain settings provided via shared memory.
 *  This method blocks until communication with the launcher is complete.
 */
void receiveSettings();

/** Identifier used for finding the communication channels to the launcher. */
qint64 launcherIdentifier();

/** Reset the launcher Identifier. Call this when detaching the probe. */
void resetLauncherIdentifier();

/** Sends the server address used for communication with the client back to the launcher. */
void sendServerAddress(const QUrl &addr);

/** Sends an error message if no server could be launched back to the launcher */
void sendServerLaunchError(const QString &reason);
}
}

#endif // GAMMARAY_PROBESETTINGS_H
