/*
  methodargument.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
        if (data) {
            value.metaType().destroy(data);
        }
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
        d->data = d->value.metaType().create(d->value.constData());
        Q_ASSERT(d->data);
        return QGenericArgument(d->name.constData(), d->data);
    }

    return {};
}
