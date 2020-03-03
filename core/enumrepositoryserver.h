/*
  enumrepositoryserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ENUMREPOSITORYSERVER_H
#define GAMMARAY_ENUMREPOSITORYSERVER_H

#include "gammaray_core_export.h"
#include "metaenum.h"

#include <common/enumrepository.h>

#include <QHash>

QT_BEGIN_NAMESPACE
class QMetaEnum;
QT_END_NAMESPACE

namespace GammaRay {

/*! Probe-side enum definition management.
 * Use this to obtain enum value representations that can be safely transferred to the client.
 */
class EnumRepositoryServer : public EnumRepository
{
    Q_OBJECT
public:
    ~EnumRepositoryServer() override;

    /*! Creates an EnumValue instance for the given numeric value and QMetaEnum.
     *  If not yet present, the enum definition with be registered
     *  with the EnumRepositoryServer, so that the client has access
     *  to the key/value mappings of the enum if needed.
     */
    static GAMMARAY_CORE_EXPORT EnumValue valueFromMetaEnum(int value, const QMetaEnum &me);

    /*! Creates an EnumValue instance for a given QVariant containing a registered
     *  GammaRay::MetaEnum value.
     */
    static GAMMARAY_CORE_EXPORT EnumValue valueFromVariant(const QVariant &value);

    /*! Check if the given meta type id is a known enum. */
    static GAMMARAY_CORE_EXPORT bool isEnum(int metaTypeId);

    //! @cond internal
    static GAMMARAY_CORE_EXPORT EnumRepository* create(QObject *parent); // only exported for unit tests

    template <typename Enum, typename V, std::size_t N>
    static void registerEnum(const MetaEnum::Value<V>(&lookup_table)[N], const char* name, bool flag)
    {
        if (isEnum(qMetaTypeId<Enum>()))
            return;
        QVector<EnumDefinitionElement> elements;
        elements.reserve(N);
        for (std::size_t i = 0; i < N; ++i)
            elements.push_back(EnumDefinitionElement(lookup_table[i].value, lookup_table[i].name));
        registerEnum(qMetaTypeId<Enum>(), name, elements, flag);
    }
    static GAMMARAY_CORE_EXPORT void registerEnum(int metaTypeId, const char *name, const QVector<EnumDefinitionElement> &elems, bool flag);

    static inline EnumDefinition definitionForId(EnumId id)
    {
        return s_instance->definition(id);
    }
    //! @endcond
private:
    explicit EnumRepositoryServer(QObject *parent = nullptr);
    void requestDefinition(EnumId id) override;

    EnumId m_nextId;

    static EnumRepositoryServer *s_instance;

    QHash<QByteArray, EnumId> m_nameToIdMap;
    QHash<int, EnumId> m_typeIdToIdMap;
};
}

/*! Register a MetaEnum lookup table with the EnumRepository. */
#define ER_REGISTER_ENUM(Class, Name, LookupTable) { \
    EnumRepositoryServer::registerEnum<Class::Name>(LookupTable, #Class "::" #Name, false); \
}

/*! Register a MetaEnum lookup table with the EnumRepository. */
#define ER_REGISTER_FLAGS(Class, Name, LookupTable) { \
    EnumRepositoryServer::registerEnum<Class::Name>(LookupTable, #Class "::" #Name, true); \
}

#endif // GAMMARAY_ENUMREPOSITORYSERVER_H
