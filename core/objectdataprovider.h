/*
  objectdataprovider.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTDATAPROVIDER_H
#define GAMMARAY_OBJECTDATAPROVIDER_H

#include "gammaray_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QObject;
class QString;
QT_END_NAMESPACE

namespace GammaRay {

class SourceLocation;

/** @brief Base class for custom object data providers.
 * Inherit from this and register at ObjectDataProvider, to support basic QObject information
 * retrieval from dynamic language bindings, such as QML.
 */
class GAMMARAY_CORE_EXPORT AbstractObjectDataProvider
{
public:
    virtual ~AbstractObjectDataProvider();
    virtual QString name(const QObject *obj) const = 0;
    virtual QString typeName(QObject *obj) const = 0;
    virtual SourceLocation creationLocation(QObject *obj) const = 0;
    virtual SourceLocation declarationLocation(QObject *obj) const = 0;
};

/** @brief Retrieve basic information about QObject instances. */
namespace ObjectDataProvider
{
    GAMMARAY_CORE_EXPORT void registerProvider(AbstractObjectDataProvider* provider);

    /** Returns the object name, considering possibly available information from
     * dynamic language runtimes, such as QML ids.
     */
    GAMMARAY_CORE_EXPORT QString name(const QObject *obj);

    /** Returns the type name of @p obj. */
    GAMMARAY_CORE_EXPORT QString typeName(QObject *obj);

    /** Returns the source location where this object was created, if known. */
    GAMMARAY_CORE_EXPORT SourceLocation creationLocation(QObject *obj);

    /** Returns the source location where the type of this object was declared, if known. */
    GAMMARAY_CORE_EXPORT SourceLocation declarationLocation(QObject *obj);
}

}

#endif // GAMMARAY_OBJECTDATAPROVIDER_H
