/*
  launcherfinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
