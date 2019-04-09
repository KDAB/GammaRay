/*
  objectdataprovider.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
