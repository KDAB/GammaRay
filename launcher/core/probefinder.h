/*
  probefinder.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROBEFINDER_H
#define GAMMARAY_PROBEFINDER_H

#include "gammaray_launcher_export.h"

#include <QStringList>
#include <QVector>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeABI;

/*! Functions to locate a suitable probe. */
namespace ProbeFinder {
/*!
 * Attempts to find the full path of the probe DLL with ABI @p probeAbi, considering
 * the additional search paths @p searchRoots.
 */
GAMMARAY_LAUNCHER_EXPORT QString findProbe(const ProbeABI &probeAbi,
                                           const QStringList &searchRoots = QStringList());

/*!
 * Find the best matching probe for the given @p targetABI out of all installed ones.
 * An invalid ProbeABI instance is returned if there is no compatible probe installed.
 */
GAMMARAY_LAUNCHER_EXPORT ProbeABI findBestMatchingABI(const ProbeABI &targetABI);
/*!
 * Find the best matching probe for the given @p targetABI out of @p availableABIs.
 * An invalid ProbeABI instance is returned if there is no compatible probe installed.
 */
GAMMARAY_LAUNCHER_EXPORT ProbeABI findBestMatchingABI(const ProbeABI &targetABI,
                                                      const QVector<ProbeABI> &availableABIs);

/*!
 * List all available probe ABIs.
 */
GAMMARAY_LAUNCHER_EXPORT QVector<ProbeABI> listProbeABIs();
}
}

#endif // GAMMARAY_PROBEFINDER_H
