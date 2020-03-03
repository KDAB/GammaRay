/*
  enumvalue.h

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

#ifndef GAMMARAY_ENUMVALUE_H
#define GAMMARAY_ENUMVALUE_H

#include "gammaray_common_export.h"

#include <QDataStream>
#include <QMetaType>

namespace GammaRay {

/*! Unique identifier for EnumDefinition instances. */
typedef int EnumId;
/*! Special value constants for GammaRay::EnumId. */
enum EnumIds {
    InvalidEnumId = -1,
    FirstValidEnumId = 0
};

/*! A target-independent representation of an enum or flag value. */
class GAMMARAY_COMMON_EXPORT EnumValue
{
public:
    EnumValue() = default;
    /*! Create a new enum value for EnumDefinition with @p id and value @p value. */
    explicit EnumValue(EnumId id, int value);

    /*! The unique identifier of the definition of this enum.
     *  @see EnumDefinition
     */
    EnumId id() const;

    /*! Returns the numeric value of this enum value. */
    int value() const;
    /*! Sets the numeric value of this enum value. */
    void setValue(int value);

    /*! Returns @c true if this instance refers to a valid EnumDefinition. */
    bool isValid() const;

private:
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumValue &v);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumValue &v);

    EnumId m_id = InvalidEnumId;
    int m_value = 0;
};

///@cond internal
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumValue &v);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumValue &v);
///@endcond

}

Q_DECLARE_METATYPE(GammaRay::EnumValue)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::EnumValue, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

#endif // GAMMARAY_ENUMVALUE_H
