/*
  enumdefinition.h

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
    EnumDefinitionElement();
    EnumDefinitionElement(int value, const char *name);

    int value() const;
    QByteArray name() const;

private:
    friend QDataStream &operator<<(QDataStream &out, const EnumDefinitionElement &elem);
    friend QDataStream &operator>>(QDataStream &in, EnumDefinitionElement &elem);

    int m_value;
    QByteArray m_name;
};

/*! Target-independent representation of an enum or flag definition. */
class GAMMARAY_COMMON_EXPORT EnumDefinition
{
public:
    EnumDefinition();
    explicit EnumDefinition(EnumId id, const QByteArray &name);

    bool isValid() const;

    EnumId id() const;
    QByteArray name() const;
    bool isFlag() const;
    void setIsFlag(bool isFlag);

    QVector<EnumDefinitionElement> elements() const;
    void setElements(const QVector<EnumDefinitionElement> &elements);

private:
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumDefinition &def);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumDefinition &def);

    EnumId m_id;
    bool m_isFlag;
    QByteArray m_name;
    QVector<EnumDefinitionElement> m_elements;
};

GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const EnumDefinition &def);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, EnumDefinition &def);

}

Q_DECLARE_METATYPE(GammaRay::EnumDefinition)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::EnumDefinitionElement, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(GammaRay::EnumDefinition, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

#endif // GAMMARAY_ENUMDEFINITION_H
