/*
  probeabidetector_win.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QDebug>
#include <QString>

#include <windows.h>
#include <tlhelp32.h>
#include <winnt.h>

using namespace GammaRay;

ProbeABI ProbeABIDetector::abiForExecutable(const QString& path) const
{
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid) const
{
  MODULEENTRY32 me;
  me.dwSize = sizeof(MODULEENTRY32);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return ProbeABI();
  }

  for (bool hasNext = Module32First(snapshot, &me); hasNext; hasNext = Module32Next(snapshot, &me)) {
    const QString module = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szModule));
    if (module.contains("QtCore") || module.contains("Qt5Core")) {
      const QString path = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szExePath));
      CloseHandle(snapshot);
      return abiForQtCore(path);
    }
  }

  CloseHandle(snapshot);
  return ProbeABI();
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString& path) const
{
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}
