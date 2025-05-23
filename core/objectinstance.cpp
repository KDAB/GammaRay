/*
  objectinstance.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "objectinstance.h"
#include "metaobjectrepository.h"
#include "metaobject.h"
#include "util.h"

using namespace GammaRay;

ObjectInstance::ObjectInstance(QObject *obj)
    : m_qtObj(obj)
    , m_type(QtObject)
{
    m_metaObj = obj ? obj->metaObject() : nullptr;
}

ObjectInstance::ObjectInstance(void *obj, const QMetaObject *metaObj)
    : m_obj(obj)
    , m_metaObj(metaObj)
    , m_type(obj ? QtGadgetPointer : QtMetaObject)
{
}

ObjectInstance::ObjectInstance(void *obj, const char *typeName)
    : m_obj(obj)
    , m_typeName(typeName)
    , m_type(Object)
{
}

ObjectInstance::ObjectInstance(const QVariant &value)
    : m_type(QtVariant)
{
    m_variant = value;
    if (value.canConvert<QObject *>()) {
        m_qtObj = value.value<QObject *>();
        if (m_qtObj) {
            m_metaObj = m_qtObj->metaObject();
            m_type = QtObject;
        }
    } else {
        auto mt = QMetaType(value.userType());
        if (mt.flags() & QMetaType::IsGadget) {
            m_metaObj = mt.metaObject();
            if (m_metaObj)
                m_type = QtGadgetValue;
        } else {
            unpackVariant();
        }
    }
}

ObjectInstance::ObjectInstance(const ObjectInstance &other)
{
    copy(other);
}

ObjectInstance &ObjectInstance::operator=(const ObjectInstance &other)
{
    copy(other);
    return *this;
}

bool ObjectInstance::operator==(const ObjectInstance &rhs) const
{
    if (type() != rhs.type())
        return false;
    switch (type()) {
    case Invalid:
        return false;
    case QtObject:
    case QtGadgetPointer:
    case Object:
        return object() == rhs.object();
    case QtMetaObject:
        return metaObject() == rhs.metaObject();
    case Value:
    case QtVariant:
    case QtGadgetValue:
        return variant() == rhs.variant();
    }

    Q_ASSERT(false);
    return false;
}

ObjectInstance::Type ObjectInstance::type() const
{
    return m_type;
}

bool ObjectInstance::isValueType() const
{
    return m_type == Value || m_type == QtGadgetValue; // ### || m_type == QtVariant; ??
}

QObject *ObjectInstance::qtObject() const
{
    return m_qtObj;
}

void *ObjectInstance::object() const
{
    Q_ASSERT(m_type == QtObject || m_type == QtGadgetPointer || m_type == QtGadgetValue || m_type == Object || m_type == Value);
    switch (m_type) {
    case QtObject:
        return m_qtObj;
    case QtGadgetPointer:
    case QtGadgetValue:
        return m_obj ? m_obj : const_cast<void *>(m_variant.constData());
    default:
        return m_obj;
    }
    Q_ASSERT(false);
    return nullptr;
}

const QVariant &ObjectInstance::variant() const
{
    Q_ASSERT(m_type == QtVariant || isValueType());
    return m_variant;
}

const QMetaObject *ObjectInstance::metaObject() const
{
    return m_metaObj;
}

QByteArray ObjectInstance::typeName() const
{
    if (m_metaObj)
        return m_metaObj->className();
    if (m_variant.isValid() && m_typeName.isEmpty())
        return m_variant.typeName();
    return m_typeName;
}

bool ObjectInstance::isValid() const
{
    switch (m_type) {
    case Invalid:
        return false;
    case QtObject:
        return m_qtObj;
    case QtMetaObject:
        return m_metaObj;
    default:
        break;
    }
    return true;
}

void ObjectInstance::copy(const ObjectInstance &other)
{
    m_obj = other.m_obj;
    m_qtObj = other.m_qtObj.data();
    m_variant = other.m_variant;
    m_metaObj = other.m_metaObj;
    m_typeName = other.m_typeName;
    m_type = other.m_type;

    if (m_type == Value || m_type == QtGadgetPointer)
        unpackVariant(); // pointer changes when copying the variant
}

void ObjectInstance::unpackVariant()
{
    const auto mo = MetaObjectRepository::instance()->metaObject(m_variant.typeName());
    if (mo && strstr(m_variant.typeName(), "*") != nullptr) { // pointer types
        QMetaType(m_variant.userType()).construct(&m_obj, m_variant.constData());
        if (!Util::isNullish(m_obj)) {
            m_type = Object;
            m_typeName = m_variant.typeName();
        }
    } else if (mo) { // value types
        m_obj = const_cast<void *>(m_variant.constData());
        m_type = Value;
        m_typeName = m_variant.typeName();
    }

    if (!m_variant.isNull() && strstr(m_variant.typeName(), "*") != nullptr) { // pointer to gadget
        QByteArray normalizedTypeName = m_variant.typeName();
        // krazy:cond=doublequote_chars
        normalizedTypeName.replace('*', "");
        normalizedTypeName.replace('&', "");
        normalizedTypeName.replace("const ", "");
        normalizedTypeName.replace(" const", "");
        normalizedTypeName.replace(' ', "");
        // krazy:endcond=doublequote_chars

        const auto typeId = QMetaType::fromName(normalizedTypeName).id();
        if (typeId != QMetaType::UnknownType && (QMetaType(typeId).flags() & QMetaType::IsGadget)) {
            QMetaType(m_variant.userType()).construct(&m_obj, m_variant.constData());
            m_metaObj = QMetaType(typeId).metaObject();
            if (m_obj && m_metaObj) {
                m_type = QtGadgetPointer;
                m_typeName = m_variant.typeName();
            }
        }
    }
}
