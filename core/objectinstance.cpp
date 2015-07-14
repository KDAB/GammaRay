/*
  objectinstance.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectinstance.h"

using namespace GammaRay;

ObjectInstance::ObjectInstance() :
    m_metaObj(0),
    m_type(Invalid)
{
    m_payload.obj = 0;
}

ObjectInstance::ObjectInstance(QObject* obj) :
    m_type(QtObject)
{
    m_payload.qtObj = obj;
    m_metaObj = obj ? obj->metaObject() : 0;
}

ObjectInstance::ObjectInstance(void* obj, const QMetaObject* metaObj) :
    m_metaObj(metaObj),
    m_type(QtGadget)
{
    m_payload.obj = obj;
}

ObjectInstance::ObjectInstance(void* obj, const char* typeName) :
    m_metaObj(0),
    m_typeName(typeName),
    m_type(Object)
{
    m_payload.obj = obj;
}

ObjectInstance::ObjectInstance(const QVariant& value) :
    m_metaObj(0),
    m_type(QtVariant)
{
    m_variant = value;
    if (value.canConvert<QObject*>()) {
        m_payload.qtObj = value.value<QObject*>();
        if (m_payload.qtObj) {
            m_metaObj = m_payload.qtObj->metaObject();
            m_type = QtObject;
        }
    } else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        m_metaObj = QMetaType::metaObjectForType(value.userType());
        if (m_metaObj) {
            m_payload.obj = const_cast<void*>(value.data());
            m_type = QtGadget;
        }
#endif
    }
}

ObjectInstance::Type ObjectInstance::type() const
{
    return m_type;
}

QObject* ObjectInstance::qtObject() const
{
    Q_ASSERT(m_type == QtObject);
    return m_payload.qtObj;
}

void* ObjectInstance::object() const
{
    Q_ASSERT(m_type == QtObject || m_type == QtGadget || m_type == Object);
    if (m_type == QtObject)
        return m_payload.qtObj;
    return m_payload.obj;
}

QVariant ObjectInstance::variant() const
{
    Q_ASSERT(m_type == QtVariant);
    return m_variant;
}

const QMetaObject* ObjectInstance::metaObject() const
{
    return m_metaObj;
}

QByteArray ObjectInstance::typeName() const
{
    Q_ASSERT(m_type == QtObject || m_type == QtGadget || m_type == Object);
    if (m_metaObj)
        return m_metaObj->className();
    return m_typeName;
}
