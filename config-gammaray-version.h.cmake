/*
  config-gammaray-version.h.cmake

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef CONFIG_GAMMARAY_VERSION_H
#define CONFIG_GAMMARAY_VERSION_H

#define GAMMARAY_VERSION_MAJOR ${GAMMARAY_VERSION_MAJOR}
#define GAMMARAY_VERSION_MINOR ${GAMMARAY_VERSION_MINOR}
#define GAMMARAY_VERSION_PATCH ${GAMMARAY_VERSION_PATCH}
#define GAMMARAY_VERSION_STRING "${GAMMARAY_VERSION_STRING}"
#define GAMMARAY_COMPACT_VERSION_STRING "${GAMMARAY_VERSION_MAJOR}.${GAMMARAY_VERSION_MINOR}.${GAMMARAY_VERSION_PATCH}"

#endif
