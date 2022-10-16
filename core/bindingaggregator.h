/*
  bindingaggregator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_BINDINGAGGREGATOR_H
#define GAMMARAY_BINDINGAGGREGATOR_H

// Own
#include <core/propertycontrollerextension.h>
#include "gammaray_core_export.h"

// Qt
#include <QObject>
#include <QPointer>

// Std
#include <memory>
#include <vector>

namespace GammaRay {
class AbstractBindingProvider;
class BindingNode;

namespace BindingAggregator {
GAMMARAY_CORE_EXPORT bool providerAvailableFor(QObject *object);
GAMMARAY_CORE_EXPORT std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(BindingNode *node);
GAMMARAY_CORE_EXPORT std::vector<std::unique_ptr<BindingNode>> bindingTreeForObject(QObject *obj);
GAMMARAY_CORE_EXPORT void scanForBindingLoops();

GAMMARAY_CORE_EXPORT void registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider);
}
}

#endif // GAMMARAY_BINDINGAGGREGATOR_H
