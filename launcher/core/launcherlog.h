/*
  launcherlog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LAUNCHERLOG_H
#define GAMMARAY_LAUNCHERLOG_H

#include <QLoggingCategory>

namespace GammaRay {
// how the target's ABI was detected and which probe was chosen, enable with
// QT_LOGGING_RULES="gammaray.launcher.info=true"; info as debug output is off in release builds
Q_DECLARE_LOGGING_CATEGORY(launcherLog)
}

#endif // GAMMARAY_LAUNCHERLOG_H
