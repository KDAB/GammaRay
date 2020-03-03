/*
  enumdefinition.h

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

#ifndef GAMMARAY_ENUMDEFINITION_H
#define GAMMARAY_ENUMDEFINITION_H

#include "gammaray_common_export.h"

#include "enumvalue.h"

#include <QVector>

namespace GammaRay {

/*! A single element of an EnumDefinition. */
class GAMMARAY_COMMON_EXPORT EnumDefinitionElement
{
public:
    EnumDefinitionElement() = default;
    /*! Create a new enum definition element with name @p name and value @p value. */
    EnumDefinitionElement(int value, const char *name);

    /*! The numeric value represented of this enum definition element. */
    int value() const;
    /*! The name of this enum definition element. */
    QByteArray name() const;

private:
    friend QDataStream &operator<<(QDataStream &out, const EnumDefinitionElement &elem);
    friend QDataStream &operator>>(QDataStream &in, EnumDefinitionElement &elem);

    int m_value = 0;
    QByteArray m_name;
};

/*! Target-independent representation of an enum or flag definition. */
class GAMMARAY_COMMON_EXPORT EnumDefinition
{
public:
    EnumDefinition() = default;
    /*! Create a new definition for an enum named @p name and internal id @p id. */
    explicit EnumDefinition(EnumId id, const QByteArray &name);

    /*! Returns whether this is a valid enum definition.
     * This means it's a enum definition registered with the EnumRepository
     * and there is at least one enum definition element present.
     */
    bool isValid() const;

    /*! A unique identifer for this enum definition.
     * This is used to identify enums between the GammaRay client and server.
     */
    EnumId id() const;
    /*! The name of the enum. */
    QByteArray name() const;
    /*! Returns @c true if this enum is used as a flag. */
    bool isFlag() const;

    /*! Returns the individual elements of this enum.
     *  That is, the key/value pairs of its definition.
     */
    QVector<EnumDefinitionElement> elements() const;

    //! @cond internal
    void setElements(const QVector<EnumDefinitionElement> &elements);
    void setIsFlag(bool isFlag);
    //! @endcond

    /*! Converts the given enum value into a string representation. */
    QByteArray valueToString(const EnumValue &value) const;

private:
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumDefinition &def);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumDefinition &def);

    EnumId m_id = InvalidEnumId;
    bool m_isFlag = false;
    QByteArray m_name;
    QVector<EnumDefinitionElement> m_elements;
};

///@cond internal
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumDefinition &def);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumDefinition &def);
///@endcond

}

Q_DECLARE_METATYPE(GammaRay::EnumDefinition)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::EnumDefinitionElement, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(GammaRay::EnumDefinition, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

#endif // GAMMARAY_ENUMDEFINITION_H
