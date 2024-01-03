/*
  objectdataprovider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

/*! Base class for custom object data providers.
 * Inherit from this and register at ObjectDataProvider, to support basic QObject information
 * retrieval from dynamic language bindings, such as QML.
 */
class GAMMARAY_CORE_EXPORT AbstractObjectDataProvider
{
public:
    AbstractObjectDataProvider();
    virtual ~AbstractObjectDataProvider();
    /*! Returns a name or identifier for @p obj. */
    virtual QString name(const QObject *obj) const = 0;
    /*! Returns the full name of the type of @p obj. */
    virtual QString typeName(const QObject *obj) const = 0;
    /*! Returns a shortened type name (e.g. excluding namespaces) for @p obj. */
    virtual QString shortTypeName(const QObject *obj) const = 0;
    /*! Returns the source location where @p obj has been created. */
    virtual SourceLocation creationLocation(const QObject *obj) const = 0;
    /*! Returns the source location where the type of @p obj has been declared. */
    virtual SourceLocation declarationLocation(const QObject *obj) const = 0;

private:
    Q_DISABLE_COPY(AbstractObjectDataProvider)
};

/*! Retrieve basic information about QObject instances. */
namespace ObjectDataProvider {
/*! Register an additional object data provider. */
GAMMARAY_CORE_EXPORT void registerProvider(AbstractObjectDataProvider *provider);

/*! Returns the object name, considering possibly available information from
 * dynamic language runtimes, such as QML ids.
 */
GAMMARAY_CORE_EXPORT QString name(const QObject *obj);

/*! Returns the type name of @p obj. */
GAMMARAY_CORE_EXPORT QString typeName(const QObject *obj);

/*! Returns the short type name of @p obj. */
GAMMARAY_CORE_EXPORT QString shortTypeName(const QObject *obj);

/*! Returns the source location where this object was created, if known. */
GAMMARAY_CORE_EXPORT SourceLocation creationLocation(const QObject *obj);

/*! Returns the source location where the type of this object was declared, if known. */
GAMMARAY_CORE_EXPORT SourceLocation declarationLocation(const QObject *obj);
}
}

#endif // GAMMARAY_OBJECTDATAPROVIDER_H
