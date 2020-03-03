/*
  paths.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PATHS_H
#define GAMMARAY_PATHS_H

#include "gammaray_common_export.h"

#include <QString>

namespace GammaRay {
/*! Methods to determine install locations of the various bits of GammaRay. */
namespace Paths {
/*! Absolute path to the root of the GammaRay installation, anything else
 * is relative to this.
 */
GAMMARAY_COMMON_EXPORT QString rootPath();

/*! Call this to set the root of the GammaRay install location, as early as
 *  possible. The install location is usually determined by using
 *  QCoreApplication::applicationDirPath() or the probe path, and the known relative
 *  install location of the corresponding component.
 */
GAMMARAY_COMMON_EXPORT void setRootPath(const QString &rootPath);

/*! Convenience version of the above, for root paths relative to
 * QCoreApplication::applicationDirPath().
 */
GAMMARAY_COMMON_EXPORT void setRelativeRootPath(const char *relativeRootPath);

/*! Returns the probe and plugin base path for the given probe ABI.
 * If @p probeABI is empty, the path containing the probes is returned.
 */
GAMMARAY_COMMON_EXPORT QString probePath(const QString &probeABI,
                                         const QString &rootPath = Paths::rootPath());

/*! Returns the path containing the GammaRay injector executable. */
GAMMARAY_COMMON_EXPORT QString binPath();

/*! Returns the path containing helper executables. */
GAMMARAY_COMMON_EXPORT QString libexecPath();

/*! Returns the path to the current probe location, probePath(GAMMARAY_PROBE_ABI).
 *  @note Must not be used outside of probe code!
 *  @deprecated Use Paths::probePath instead.
 */
GAMMARAY_COMMON_DEPRECATED_EXPORT QString currentProbePath();

/*! Returns a list of probe plugin search paths for the given ABI.
 *  This includes candidates based on Qt's plugin search paths, and GammaRay's
 *  install layout.
 *  @param probeABI The ABI identifier we are looking for plugins for. If you
 *   are actually loading plugins, this must be GAMMARAY_PROBE_ABI.
 */
GAMMARAY_COMMON_EXPORT QStringList pluginPaths(const QString &probeABI);

/*! Returns a list of target plugin search paths for the given ABI.
 *  This includes candidates based on Qt's plugin search paths, and GammaRay's
 *  install layout.
 *  @param probeABI The ABI identifier we are looking for plugins for. If you
 *   are actually loading plugins, this must be GAMMARAY_PROBE_ABI.
 */
GAMMARAY_COMMON_EXPORT QStringList targetPluginPaths(const QString &probeABI);

/*! Returns the path to the current plugins location.
 *  This is the same as currentProbePath() when using the GammaRay installation layout,
 *  but differs with e.g. the Android installation layout.
 *  @note Must not be used outside of probe or client/ui code!
 *  @deprecated Use Paths::pluginsPaths instead.
 */
GAMMARAY_COMMON_DEPRECATED_EXPORT QString currentPluginsPath();

/*! Returns the file extension used on the current platform for libraries. */
GAMMARAY_COMMON_EXPORT QString libraryExtension();

/*! Returns the file extension used on the current platform for plugins. */
GAMMARAY_COMMON_EXPORT QString pluginExtension();

/*! Path to the installed .qch/.qhc documentation files. */
GAMMARAY_COMMON_EXPORT QString documentationPath();
}
}

#endif // GAMMARAY_PATHS_H
