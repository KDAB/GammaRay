/*
  metaobjectrepository.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_METAOBJECTREPOSITORY_H
#define GAMMARAY_METAOBJECTREPOSITORY_H

#include <QHash>

class QString;

namespace GammaRay {

class MetaObject;

/**
 * Repository of compile-time introspection information for stuff
 * not covered by the Qt meta object system.
 */
class MetaObjectRepository
{
  public:
    ~MetaObjectRepository();

    /** Singleton accessor. */
    static MetaObjectRepository *instance();

    /**
     * Adds object type information to the repository.
     */
    void addMetaObject(MetaObject *mo);

    /**
     * Returns the introspection information for the type with the given name.
     */
    MetaObject *metaObject(const QString &typeName) const;

    /**
     * Returns whether a meta object is known for the given type name.
     */
    bool hasMetaObject(const QString &typeName) const;

  protected:
    MetaObjectRepository();

  private:
    void initBuiltInTypes();
    void initQObjectTypes();
    void initGraphicsViewTypes();

  private:
    QHash<QString, MetaObject*> m_metaObjects;
};

}

#endif // GAMMARAY_METAOBJECTREPOSITORY_H
