/*
  probesettings.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
