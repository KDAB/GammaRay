/*
  probesettings.cpp

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

#include "probesettings.h"

using namespace GammaRay;

QVariant ProbeSettings::value(const QString& key, const QVariant& defaultValue)
{
  // TODO: for now we simply use environment variables, which only works for launching, not attaching
  // this eventually needs to be extended to eg. shared memory or temporary files as communication channel

  const QByteArray v = qgetenv("GAMMARAY_" + key.toLocal8Bit());
  if (v.isEmpty())
    return defaultValue;

  switch (defaultValue.type()) {
    case QVariant::String:
      return QString::fromUtf8(v);
    case QVariant::Bool:
      return v == "true" || v == "1" || v == "TRUE";
    case QVariant::Int:
      return v.toInt();
    default:
      return v;
  }
}