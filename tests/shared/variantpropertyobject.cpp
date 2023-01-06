/*
  variantpropertyobject.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "variantpropertyobject.h"

#include <QHash>
#include <QVariant>

VariantPropertyObject::VariantPropertyObject(QObject *parent)
    : QObject(parent)
    , m_object(new QObject)
{
    QHash<QString, int> mapping;
    mapping.insert(QStringLiteral("One"), 1);
    mapping.insert(QStringLiteral("Two"), 2);

    setProperty("dynamicProperty", QVariant::fromValue(mapping));
}

VariantPropertyObject::~VariantPropertyObject() = default;

QSharedPointer<QObject> VariantPropertyObject::sharedObject() const
{
    return m_object;
}

QPointer<QObject> VariantPropertyObject::trackingObject() const
{
    return m_object.data();
}

QVector<int> VariantPropertyObject::widgetVector()
{
    QVector<int> vec;
    vec << 5;
    vec << 6;
    vec << 7;
    vec << 8;
    return vec;
}
