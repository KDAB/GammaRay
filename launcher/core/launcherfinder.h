/*
  launcherfinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LAUNCHERFINDER_H
#define GAMMARAY_LAUNCHERFINDER_H

#include "gammaray_launcher_export.h"
#include "probeabi.h"

#include <QString>


namespace GammaRay {
/*! Utility function to find the various executables related to the launcher. */
namespace LauncherFinder {
/*! Identifies a launcher helper executable type. */
enum Type
{
    Injector,
    LauncherUI,
    Client
};

/*! Returns the path to the helper executable of type @p type. */
GAMMARAY_LAUNCHER_EXPORT QString findLauncher(Type type);
}
}

#endif // GAMMARAY_LAUNCHERFINDER_H
