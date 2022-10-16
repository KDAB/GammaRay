/*
  propertyfilter.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYFILTER_H
#define GAMMARAY_PROPERTYFILTER_H

#include "gammaray_core_export.h"
#include "propertydata.h"

#include <QFlags>
#include <QString>
#include <QVariant>

namespace GammaRay {
class GAMMARAY_CORE_EXPORT PropertyFilter
{
public:
    explicit PropertyFilter()
    {
    }
    explicit PropertyFilter(
        const QString &className,
        const QString &name,
        const QString &typeName = QString(),
        PropertyData::AccessFlags accessFlags = {},
        PropertyModel::PropertyFlags propertyFlags = {});
    static PropertyFilter classAndPropertyName(const QString &className,
                                               const QString &propertyName);

    bool matches(const PropertyData &prop) const;

private:
    QString m_name;
    QString m_typeName;
    QString m_className;
    PropertyData::AccessFlags m_accessFlags;
    PropertyModel::PropertyFlags m_propertyFlags;
};

namespace PropertyFilters {
GAMMARAY_CORE_EXPORT bool matches(const PropertyData &prop);

/**
 * Register a filter to remove properties from the property view. Property
 * adaptors are free to consider or ignore filters.
 *
 * Use cases are e.g.:
 * * Removing a QMetaProperty in order to re-add it from a custom adaptor
 * * Prohibiting a property getter to be called, if it's known to crash the
 *   application under certain circumstances (e.g. if a getter requires an
 *   OpenGL context to be valid)
 */
GAMMARAY_CORE_EXPORT void registerFilter(const PropertyFilter &filter);
}

}

#endif // GAMMARAY_PROPERTYFILTER_H
