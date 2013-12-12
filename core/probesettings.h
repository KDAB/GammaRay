/*
  probesettings.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

namespace GammaRay {

/** General purpose settings of the probe provided by the launcher. */
namespace ProbeSettings
{
  GAMMARAY_CORE_EXPORT QVariant value(const QString &key, const QVariant &defaultValue = QString());

  /** Call if using runtime attaching to obtain settings provided via shared memory. */
  void receiveSettings();

  /** Identifier used for finding the communication channels to the launcher. */
  qint64 launcherIdentifier();

  /** Sends the TCP port used for communication with the client back to the launcher. */
  void sendPort(quint16 port);
}

}

#endif // GAMMARAY_PROBESETTINGS_H
