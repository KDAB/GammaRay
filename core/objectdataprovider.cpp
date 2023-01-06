/*
  objectdataprovider.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectdataprovider.h"
#include "probe.h"

#include <common/sourcelocation.h>

#include <QObject>
#include <QVector>

namespace GammaRay {

AbstractObjectDataProvider::AbstractObjectDataProvider() = default;

AbstractObjectDataProvider::~AbstractObjectDataProvider() = default;

Q_GLOBAL_STATIC(QVector<AbstractObjectDataProvider *>, s_providers)

void ObjectDataProvider::registerProvider(AbstractObjectDataProvider *provider)
{
    if (!s_providers()->contains(provider))
        s_providers()->push_back(provider);
}

QString ObjectDataProvider::name(const QObject *obj)
{
    if (!obj)
        return QStringLiteral("0x0");
    auto name = obj->objectName();
    if (!name.isEmpty())
        return name;
    foreach (auto provider, *s_providers()) {
        name = provider->name(obj);
        if (!name.isEmpty())
            return name;
    }
    return name;
}

QString ObjectDataProvider::typeName(QObject *obj)
{
    if (!obj)
        return QString();

    foreach (auto provider, *s_providers()) {
        const auto name = provider->typeName(obj);
        if (!name.isEmpty())
            return name;
    }
    return obj->metaObject()->className();
}

QString ObjectDataProvider::shortTypeName(QObject *obj)
{
    if (!obj)
        return QString();

    foreach (auto provider, *s_providers()) {
        const auto name = provider->shortTypeName(obj);
        if (!name.isEmpty())
            return name;
    }
    return obj->metaObject()->className();
}

SourceLocation ObjectDataProvider::creationLocation(QObject *obj)
{
    SourceLocation loc;
    if (!obj)
        return loc;

    foreach (auto provider, *s_providers()) {
        loc = provider->creationLocation(obj);
        if (loc.isValid())
            return loc;
    }

    loc = Probe::instance()->objectCreationSourceLocation(obj);
    return loc;
}

SourceLocation ObjectDataProvider::declarationLocation(QObject *obj)
{
    SourceLocation loc;
    if (!obj)
        return loc;

    foreach (auto provider, *s_providers()) {
        loc = provider->declarationLocation(obj);
        if (loc.isValid())
            return loc;
    }

    return loc;
}
}
