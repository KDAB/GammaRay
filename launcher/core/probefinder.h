/*
  probefinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
