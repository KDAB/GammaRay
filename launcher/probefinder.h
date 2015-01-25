/*
  probefinder.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROBEFINDER_H
#define GAMMARAY_PROBEFINDER_H

#include <QVector>

class QString;
class QStringList;

namespace GammaRay {

class ProbeABI;

namespace ProbeFinder {

  /**
   * Attempts to find the full path of the probe DLL.
   */
  QString findProbe(const QString& baseName, const ProbeABI& probeAbi);

  /**
   * Find the best matching probe for the given @p targetABI.
   * An invalid ProbeABI instance is returned if there is no compatible probe installed.
   */
  ProbeABI findBestMatchingABI(const ProbeABI &targetABI);
  ProbeABI findBestMatchingABI(const ProbeABI &targetABI, const QVector<ProbeABI> &availableABIs);

  /**
   * List all available probe ABIs.
   */
  QVector<ProbeABI> listProbeABIs();

  /**
   * Returns the platform-specific file name extenstion.
   */
  QString fileExtension();
}

}

#endif // GAMMARAY_PROBEFINDER_H
