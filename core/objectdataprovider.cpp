/*
  objectdataprovider.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

QString ObjectDataProvider::typeName(const QObject *obj)
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

QString ObjectDataProvider::shortTypeName(const QObject *obj)
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

SourceLocation ObjectDataProvider::creationLocation(const QObject *obj)
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

SourceLocation ObjectDataProvider::declarationLocation(const QObject *obj)
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
