/*
  probeabidetector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROBEABIDETECTOR_H
#define GAMMARAY_PROBEABIDETECTOR_H

#include "gammaray_launcher_export.h"

#include "probeabi.h"

#include <QHash>
#include <QString>

namespace GammaRay {
/*! Detect the probe ABI required for a given target.
 *  A target can be specified as either a process id or a path to an executable to be launched.
 */
class GAMMARAY_LAUNCHER_EXPORT ProbeABIDetector
{
public:
    ProbeABIDetector();

    /*! Detect the ABI of the executable at @p path. */
    ProbeABI abiForExecutable(const QString &path) const;

    /*! Detect the ABI of the process running with PID @p pid. */
    ProbeABI abiForProcess(qint64 pid) const;

    /*! Returns the full path to QtCore used by the given executable. */
    QString qtCoreForExecutable(const QString &path) const;

    /*! Returns the full path to QtCore used by the process with PID @p pid. */
    QString qtCoreForProcess(quint64 pid) const;

    ///@cond internal
    /*! Check if the given line contains a mention of the QtCore library.
     *  @internal
     */
    static bool containsQtCore(const QByteArray &line);
    ///@endcond

private:
    /*! Returns the ABI of the given QtCore DLL.
     *  Implementation of abiForXXX() should call this as it implements caching.
     */
    ProbeABI abiForQtCore(const QString &path) const;

    /*! Detect the ABI of the given QtCore DLL.
     *  This needs to be implemented for every platform.
     */
    ProbeABI detectAbiForQtCore(const QString &path) const;

    /*! Path to the QtCore DLL for @p pid, using the lsof tool
     *  on UNIX-like systems.
     */
    QString qtCoreFromLsof(qint64 pid) const;

    mutable QHash<QString, ProbeABI> m_abiForQtCoreCache;
};
}

#endif // GAMMARAY_PROBEABIDETECTOR_H
