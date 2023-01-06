/*
  propertyfilter.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreukamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertyfilter.h"
#include <QVector>

using namespace GammaRay;

Q_GLOBAL_STATIC(QVector<PropertyFilter>, s_propertyFilters)

GammaRay::PropertyFilter::PropertyFilter(
    const QString &className,
    const QString &name,
    const QString &typeName,
    PropertyData::AccessFlags accessFlags,
    PropertyModel::PropertyFlags propertyFlags)
    : m_name(name)
    , m_typeName(typeName)
    , m_className(className)
    , m_accessFlags(accessFlags)
    , m_propertyFlags(propertyFlags)
{
}

GammaRay::PropertyFilter GammaRay::PropertyFilter::classAndPropertyName(const QString &className,
                                                                        const QString &propertyName)
{
    return PropertyFilter(className, propertyName);
}

bool GammaRay::PropertyFilter::matches(const GammaRay::PropertyData &prop) const
{
    if (!m_className.isEmpty() && prop.className() != m_className) {
        return false;
    }
    if (!m_name.isEmpty() && prop.name() != m_name) {
        return false;
    }
    if (!m_typeName.isEmpty() && prop.typeName() != m_typeName) {
        return false;
    }
    if (m_accessFlags && (prop.accessFlags() & m_accessFlags) == m_accessFlags) {
        return false;
    }
    if (m_propertyFlags && (prop.propertyFlags() & m_propertyFlags) == m_propertyFlags) {
        return false;
    }
    return true;
}

bool GammaRay::PropertyFilters::matches(const GammaRay::PropertyData &prop)
{
    return std::any_of(s_propertyFilters()->begin(),
                       s_propertyFilters()->end(),
                       [&prop](const PropertyFilter &filter) {
                           return filter.matches(prop);
                       });
}

void PropertyFilters::registerFilter(const PropertyFilter &filter)
{
    s_propertyFilters()->push_back(filter);
}
