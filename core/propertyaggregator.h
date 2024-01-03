/*
  propertyaggregator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYAGGREGATOR_H
#define GAMMARAY_PROPERTYAGGREGATOR_H

#include "gammaray_core_export.h"

#include "propertyadaptor.h"

#include <QVector>

namespace GammaRay {
/** Aggregates the results of a bunch of PropertyAdaptor instances. */
class GAMMARAY_CORE_EXPORT PropertyAggregator : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit PropertyAggregator(QObject *parent = nullptr);
    ~PropertyAggregator() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
    void writeProperty(int index, const QVariant &value) override;
    bool canAddProperty() const override;
    void addProperty(const PropertyData &data) override;
    void resetProperty(int index) override;

    void addPropertyAdaptor(PropertyAdaptor *adaptor);

protected:
    void doSetObject(const ObjectInstance &oi) override;

private slots:
    void slotPropertyChanged(int first, int last);
    void slotPropertyAdded(int first, int last);
    void slotPropertyRemoved(int first, int last);

private:
    QVector<PropertyAdaptor *> m_propertyAdaptors;
};
}

#endif // GAMMARAY_PROPERTYAGGREGATOR_H
