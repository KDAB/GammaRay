/*
  propertycontrollerextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertycontrollerextension.h"

using namespace GammaRay;

PropertyControllerExtension::PropertyControllerExtension(const QString &name)
    : m_name(name)
{
}

PropertyControllerExtension::~PropertyControllerExtension() = default;

QString PropertyControllerExtension::name() const
{
    return m_name;
}

bool PropertyControllerExtension::setObject(void *object, const QString &typeName)
{
    Q_UNUSED(object);
    Q_UNUSED(typeName);
    return false;
}

bool PropertyControllerExtension::setQObject(QObject *object)
{
    Q_UNUSED(object);
    return false;
}

bool PropertyControllerExtension::setMetaObject(const QMetaObject *metaObject)
{
    Q_UNUSED(metaObject);
    return false;
}
