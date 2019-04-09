/*
  propertyaggregator.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
