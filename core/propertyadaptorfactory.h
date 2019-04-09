/*
  propertyadaptorfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_PROPERTYADAPTORFACTORY_H
#define GAMMARAY_PROPERTYADAPTORFACTORY_H

#include "gammaray_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

namespace GammaRay {
class ObjectInstance;
class PropertyAdaptor;

/** Base class for custom property adaptor factories. */
class GAMMARAY_CORE_EXPORT AbstractPropertyAdaptorFactory
{
public:
    AbstractPropertyAdaptorFactory();
    virtual ~AbstractPropertyAdaptorFactory();

    /** Create a custom property adaptor if it can handle @p oi, return @c nullptr otherwise. */
    virtual PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const = 0;

private:
    Q_DISABLE_COPY(AbstractPropertyAdaptorFactory)
};

/** Factory for property adaptors. */
namespace PropertyAdaptorFactory {
/** Create a property adaptor for @p oi, if necessary wrapped in an aggregator.
 *  Can return @c nullptr.
 */
GAMMARAY_CORE_EXPORT PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr);

/** Register a custom property adaptor factory. */
GAMMARAY_CORE_EXPORT void registerFactory(AbstractPropertyAdaptorFactory *factory);
}
}

#endif // GAMMARAY_PROPERTYADAPTORFACTORY_H
