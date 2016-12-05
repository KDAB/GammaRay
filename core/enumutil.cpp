/*
  enumutil.cpp

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

#include "enumutil.h"

#include <QMetaEnum>

using namespace GammaRay;

namespace GammaRay {
class ProtectedExposer : public QObject
{
public:
    using QObject::staticQtMetaObject;
};
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
static const QMetaObject* metaObjectForClass(const QByteArray &name)
{
    if (name.isEmpty())
        return nullptr;
    auto mo = QMetaType::metaObjectForType(QMetaType::type(name));
    if (mo)
        return mo;
    mo = QMetaType::metaObjectForType(QMetaType::type(name + '*')); // try pointer version, more likely for QObjects
    return mo;
}
#endif

QMetaEnum EnumUtil::metaEnum(const QVariant &value, const char *typeName, const QMetaObject *metaObject)
{
    QByteArray enumTypeName(typeName);
    if (enumTypeName.isEmpty())
        enumTypeName = value.typeName();

    // split class name and enum name
    QByteArray className;
    const int pos = enumTypeName.lastIndexOf("::");
    if (pos >= 0) {
        className = enumTypeName.left(pos);
        enumTypeName = enumTypeName.mid(pos + 2);
    }

    const QMetaObject *mo = &ProtectedExposer::staticQtMetaObject;
    int enumIndex = mo->indexOfEnumerator(enumTypeName);
    if (enumIndex < 0 && metaObject) {
        mo = metaObject;
        enumIndex = mo->indexOfEnumerator(enumTypeName);
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (enumIndex < 0 && (mo = QMetaType::metaObjectForType(QMetaType::type(typeName)))) {
        enumIndex = mo->indexOfEnumerator(enumTypeName);
    }
    if (enumIndex < 0 && (mo = metaObjectForClass(className))) {
        enumIndex = mo->indexOfEnumerator(enumTypeName);
    }
#endif
    if (enumIndex < 0)
        return QMetaEnum();
    return mo->enumerator(enumIndex);
}

int EnumUtil::enumToInt(const QVariant &value, const QMetaEnum &me)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // QVariant has no implicit QFlag to int conversion as of Qt 5.7
    if (me.isFlag() && QMetaType::sizeOf(value.userType()) == sizeof(int)) // int should be enough, QFlag has that hardcoded
        return value.constData() ? *static_cast<const int*>(value.constData()) : 0;
#else
    Q_UNUSED(me);
#endif
    return value.toInt();
}

QString EnumUtil::enumToString(const QVariant &value, const char *typeName, const QMetaObject *metaObject)
{
    const auto me = metaEnum(value, typeName, metaObject);
    if (!me.isValid())
        return QString();
    return me.valueToKeys(enumToInt(value, me));
}
