/*
  enumrepositoryserver.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "enumrepositoryserver.h"

#include <QMetaEnum>

using namespace GammaRay;

EnumRepositoryServer *EnumRepositoryServer::s_instance = nullptr;

EnumRepositoryServer::EnumRepositoryServer(QObject *parent)
    : EnumRepository(parent)
    , m_nextId(FirstValidEnumId)
{
}

EnumRepositoryServer::~EnumRepositoryServer()
{
    s_instance = nullptr;
}

bool EnumRepositoryServer::isEnum(int metaTypeId)
{
    if (!s_instance)
        return false;
    return s_instance->m_typeIdToIdMap.contains(metaTypeId);
}


EnumRepository *EnumRepositoryServer::create(QObject *parent)
{
    Q_ASSERT(!s_instance);
    s_instance = new EnumRepositoryServer(parent);
    return s_instance;
}

void EnumRepositoryServer::requestDefinition(EnumId id)
{
    const auto def = definition(id);
    if (def.isValid())
        emit definitionResponse(definition(id));
}

EnumValue EnumRepositoryServer::valueFromMetaEnum(int value, const QMetaEnum &me)
{
    Q_ASSERT(s_instance);
    Q_ASSERT(me.isValid());

    const auto typeName = QByteArray(me.scope()) + "::" + me.name();
    const auto it = s_instance->m_nameToIdMap.constFind(typeName);
    if (it != s_instance->m_nameToIdMap.constEnd())
        return EnumValue(it.value(), value);

    EnumDefinition def(s_instance->m_nextId++, typeName);
    def.setIsFlag(me.isFlag());
    QVector<EnumDefinitionElement> elements;
    elements.reserve(me.keyCount());
    for (int i = 0; i < me.keyCount(); ++i)
        elements.push_back(EnumDefinitionElement(me.value(i), me.key(i)));
    def.setElements(elements);

    s_instance->addDefinition(def);
    s_instance->m_nameToIdMap.insert(typeName, def.id());

    return EnumValue(def.id(), value);
}

EnumValue EnumRepositoryServer::valueFromVariant(const QVariant &value)
{
    Q_ASSERT(s_instance);

    const auto it = s_instance->m_typeIdToIdMap.constFind(value.userType());
    Q_ASSERT(it != s_instance->m_typeIdToIdMap.constEnd());

    const auto def = definitionForId(it.value());
    if (def.isFlag())
        return EnumValue(it.value(), *static_cast<const int *>(value.constData())); // see EnumUtil
    return EnumValue(it.value(), value.toInt());
}

void EnumRepositoryServer::registerEnum(int metaTypeId, const char *name, const QVector<GammaRay::EnumDefinitionElement> &elems, bool flag)
{
    Q_ASSERT(s_instance);
    Q_ASSERT(name);
    Q_ASSERT(!elems.isEmpty());

    EnumDefinition def(s_instance->m_nextId++, name);
    def.setIsFlag(flag);
    def.setElements(elems);
    s_instance->addDefinition(def);
    s_instance->m_typeIdToIdMap.insert(metaTypeId, def.id());
}
