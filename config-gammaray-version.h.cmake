/*
  config-gammaray-version.h.cmake

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef CONFIG_GAMMARAY_VERSION_H
#define CONFIG_GAMMARAY_VERSION_H

#define GAMMARAY_VERSION_MAJOR ${GAMMARAY_VERSION_MAJOR}
#define GAMMARAY_VERSION_MINOR ${GAMMARAY_VERSION_MINOR}
#define GAMMARAY_VERSION_PATCH ${GAMMARAY_VERSION_PATCH}
#define GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION_STRING}"
#define GAMMARAY_COMPACT_VERSION_STRING "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}"

#endif
