/*
  paths.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PATHS_H
#define GAMMARAY_PATHS_H

#include "gammaray_common_export.h"

class QString;

namespace GammaRay {

/** Methods to determine install locations of the various bits of GammaRay. */
namespace Paths
{
  /** Absolute path to the root of the GammaRay installation, anything else
   * is relative to this.
   */
  GAMMARAY_COMMON_EXPORT QString rootPath();

  /** Call this to set the root of the GammaRay install location, as early as
   *  possible. The install location is usually determined by using
   *  QCoreApplication::applicationDirPath() or the probe path, and the known relative
   *  install location of the corresponding component.
   */
  GAMMARAY_COMMON_EXPORT void setRootPath(const QString &rootPath);

  /** Convenience version of the above, for root paths relative to
   * QCoreApplication::applicationDirPath().
   */
  GAMMARAY_COMMON_EXPORT void setRelativeRootPath(const char* relativeRootPath);

  /** Returns the probe and plugin base path for the given probe ABI.
   * If @p probeABI is empty, the path containing the probes is returned.
   */
  GAMMARAY_COMMON_EXPORT QString probePath(const QString& probeABI);

  /** Returns the path containing the GammaRay injector executable. */
  GAMMARAY_COMMON_EXPORT QString binPath();

  /** Returns the path containing helper executables. */
  GAMMARAY_COMMON_EXPORT QString libexecPath();

  /** Returns the path to the current probe location, probePath(GAMMARAY_PROBE_ABI). */
  GAMMARAY_COMMON_EXPORT QString currentProbePath();
}
}

#endif // GAMMARAY_PATHS_H
