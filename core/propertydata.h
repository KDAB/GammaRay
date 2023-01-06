/*
  propertydata.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYDATA_H
#define GAMMARAY_PROPERTYDATA_H

#include "gammaray_core_export.h"

#include <common/propertymodel.h>

#include <QString>
#include <QVariant>

namespace GammaRay {
/** Represents a single property value. */
class GAMMARAY_CORE_EXPORT PropertyData
{
public:
    enum AccessFlag
    {
        Readable = 0,
        Writable = 1,
        Deletable = 2,
        Resettable = 4
    };
    Q_DECLARE_FLAGS(AccessFlags, AccessFlag)

    PropertyData() = default;

    QString name() const;
    void setName(const QString &name);
    QVariant value() const;
    void setValue(const QVariant &value);
    QString typeName() const;
    void setTypeName(const QString &typeName);
    QString className() const;
    void setClassName(const QString &className);
    AccessFlags accessFlags() const;
    void setAccessFlags(AccessFlags flags);

    PropertyModel::PropertyFlags propertyFlags() const;
    void setPropertyFlags(PropertyModel::PropertyFlags flags);
    int revision() const;
    void setRevision(int rev);
    QString notifySignal() const;
    void setNotifySignal(const QString &notifySignal);

private:
    QString m_name;
    QVariant m_value;
    QString m_typeName;
    QString m_className;
    QString m_notifySignal;
    AccessFlags m_accessFlags = Readable;
    PropertyModel::PropertyFlags m_propertyFlags = PropertyModel::None;
    int m_revision = -1;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::PropertyData::AccessFlags)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::PropertyData, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

#endif // GAMMARAY_PROPERTYDATA_H
