/*
  injectorfactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_INJECTORFACTORY_H
#define GAMMARAY_INJECTORFACTORY_H

#include "gammaray_launcher_export.h"

#include "abstractinjector.h"

namespace GammaRay {
class ProbeABI;

namespace InjectorFactory {
GAMMARAY_LAUNCHER_EXPORT AbstractInjector::Ptr createInjector(const QString &name,
                                                              const QString &executableOverride = QString());

AbstractInjector::Ptr defaultInjectorForLaunch(const ProbeABI &abi, QStringList *errorStrings = nullptr);

AbstractInjector::Ptr defaultInjectorForAttach(QStringList *errorStrings = nullptr);

/**
 * Returns the list of available injector types.
 */
GAMMARAY_LAUNCHER_EXPORT QStringList availableInjectors();
}
}

#endif // GAMMARAY_INJECTORFACTORY_H
