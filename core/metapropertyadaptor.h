/*
  metapropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METAPROPERTYADAPTOR_H
#define GAMMARAY_METAPROPERTYADAPTOR_H

#include "propertyadaptor.h"

namespace GammaRay {
class MetaObject;

/** Property access for GammaRay meta-type based property-access. */
class MetaPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit MetaPropertyAdaptor(QObject *parent = nullptr);
    ~MetaPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
    void writeProperty(int index, const QVariant &value) override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    MetaObject *m_metaObj;
    void *m_obj;
};
}

#endif // GAMMARAY_METAPROPERTYADAPTOR_H
