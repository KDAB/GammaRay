/*
  translator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TRANSLATOR_H
#define GAMMARAY_TRANSLATOR_H

#include "gammaray_common_export.h"

#include <QLocale>

namespace GammaRay {
namespace Translator {
/** Load translation catalogs for current locale. */
void GAMMARAY_COMMON_EXPORT loadTranslations(const QString &catalog, const QString &path,
                                             const QString &overrideLanguage = QString());
void GAMMARAY_COMMON_EXPORT loadGammaRayTranslations(const QString &overrideLanguage = QString());
void GAMMARAY_COMMON_EXPORT loadStandAloneTranslations(const QString &overrideLanguage = QString());
}
}

#endif // GAMMARAY_TRANSLATOR_H
