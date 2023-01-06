/*
  probeabidetector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBEABIDETECTOR_H
#define GAMMARAY_PROBEABIDETECTOR_H

#include "gammaray_launcher_export.h"

#include "probeabi.h"

#include <QHash>
#include <QString>
#include <QVector>

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
    static QString qtCoreForExecutable(const QString &path);

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
    QVector<ProbeABI> abiForQtCore(const QString &path) const;

    /*! Detect the ABI of the given QtCore DLL.
     *  This needs to be implemented for every platform.
     */
    static QVector<ProbeABI> detectAbiForQtCore(const QString &path);

    /*! Path to the QtCore DLL for @p pid, using the lsof tool
     *  on UNIX-like systems.
     */
    static QString qtCoreFromLsof(qint64 pid);

    mutable QHash<QString, QVector<ProbeABI>> m_abiForQtCoreCache;
};
}

#endif // GAMMARAY_PROBEABIDETECTOR_H
