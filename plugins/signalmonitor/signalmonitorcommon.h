/*
  signalmonitorcommon.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SIGNALMONITORCOMMON_H
#define GAMMARAY_SIGNALMONITORCOMMON_H

#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QVector>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QVector<qlonglong>)

typedef QHash<int, QByteArray> IntByteArrayHash;
Q_DECLARE_METATYPE(IntByteArrayHash)
#endif

namespace GammaRay {

namespace StreamOperators {

void registerSignalMonitorStreamOperators();

}

}

#endif // GAMMARAY_SIGNALMONITORCOMMON_H
