/*
  methodargument.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "methodargument.h"
#include "variantwrapper.h"

#include <QSharedData>
#include <QMetaType>

using namespace GammaRay;

class GammaRay::MethodArgumentPrivate : public QSharedData
{
public:
    MethodArgumentPrivate() = default;

    MethodArgumentPrivate(const MethodArgumentPrivate &other)
        : QSharedData(other)
    {
        value = other.value;
        name = other.name;
        data = nullptr;
        unwrapVariant = other.unwrapVariant;
    }

    ~MethodArgumentPrivate()
    {
        if (data)
            QMetaType::destroy(value.userType(), data);
    }

    QVariant value;
    QByteArray name;
    void *data = nullptr;
    bool unwrapVariant = true;
};

MethodArgument::MethodArgument()
    : d(new MethodArgumentPrivate)
{
}

MethodArgument::MethodArgument(const QVariant &v)
    : d(new MethodArgumentPrivate)
{
    if (v.userType() == qMetaTypeId<VariantWrapper>()) {
        d->value = v.value<VariantWrapper>().variant();
        d->unwrapVariant = false;
        d->name = "QVariant";
    } else {
        d->value = v;
        d->unwrapVariant = true;
        d->name = v.typeName();
    }
}

MethodArgument::MethodArgument(const MethodArgument &) = default;

MethodArgument::~MethodArgument() = default;

MethodArgument &MethodArgument::operator=(const MethodArgument &) = default;

MethodArgument::operator QGenericArgument() const
{
    if (!d->unwrapVariant)
        return QGenericArgument(d->name.constData(), &d->value);

    if (d->value.isValid()) {
        d->data = QMetaType::create(d->value.userType(), d->value.constData());
        Q_ASSERT(d->data);
        return QGenericArgument(d->name.constData(), d->data);
    }

    return {};
}
