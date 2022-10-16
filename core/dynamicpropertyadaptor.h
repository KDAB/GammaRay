/*
  dynamicpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_DYNAMICPROPERTYADAPTOR_H
#define GAMMARAY_DYNAMICPROPERTYADAPTOR_H

#include "propertyadaptor.h"

namespace GammaRay {
/** Access to dynamic QObject properties. */
class DynamicPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit DynamicPropertyAdaptor(QObject *parent = nullptr);
    ~DynamicPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
    void writeProperty(int index, const QVariant &value) override;
    bool canAddProperty() const override;
    void addProperty(const PropertyData &data) override;

protected:
    void doSetObject(const ObjectInstance &oi) override;
    bool eventFilter(QObject *receiver, QEvent *event) override;

private:
    QList<QByteArray> m_propNames;
};
}

#endif // GAMMARAY_DYNAMICPROPERTYADAPTOR_H
