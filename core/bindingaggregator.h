/*
  bindingaggregator.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

namespace BindingAggregator
{
    GAMMARAY_CORE_EXPORT bool providerAvailableFor(QObject *object);
    GAMMARAY_CORE_EXPORT std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(BindingNode* node);
    GAMMARAY_CORE_EXPORT std::vector<std::unique_ptr<BindingNode>> bindingTreeForObject(QObject* obj);
    GAMMARAY_CORE_EXPORT void scanForBindingLoops();

    GAMMARAY_CORE_EXPORT void registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider);
}
}

#endif // GAMMARAY_BINDINGAGGREGATOR_H
