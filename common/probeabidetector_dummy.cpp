/*
  probeabidetector_dummy.cpp

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

#include "config-gammaray.h"

#include "probeabidetector.h"
#include "probeabi.h"

#include <QString>

using namespace GammaRay;

ProbeABI ProbeABIDetector::abiForExecutable(const QString& path) const
{
  Q_UNUSED(path);
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid) const
{
  Q_UNUSED(pid);
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString& path) const
{
  Q_UNUSED(path);
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}
