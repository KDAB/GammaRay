/*
  propertyadaptorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertyadaptorfactory.h"

#include "propertyaggregator.h"
#include "objectinstance.h"
#include "qmetapropertyadaptor.h"
#include "dynamicpropertyadaptor.h"
#include "sequentialpropertyadaptor.h"
#include "associativepropertyadaptor.h"
#include "metapropertyadaptor.h"

#include <QVector>

#include <algorithm>

using namespace GammaRay;

PropertyAdaptor* PropertyAdaptorFactory::create(const ObjectInstance& oi, QObject *parent)
{
    QVector<PropertyAdaptor*> adaptors;

    if (oi.metaObject())
        adaptors.push_back(new QMetaPropertyAdaptor(parent));
    if (oi.type() == ObjectInstance::QtObject)
        adaptors.push_back(new DynamicPropertyAdaptor(parent));
    if (oi.type() == ObjectInstance::QtObject || oi.type() == ObjectInstance::Object || oi.type() == ObjectInstance::QtGadget)
        adaptors.push_back(new MetaPropertyAdaptor(parent));

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (oi.type() == ObjectInstance::QtVariant) {
        const auto v = oi.variant();
        if (v.canConvert<QVariantList>())
            adaptors.push_back(new SequentialPropertyAdaptor(parent));
        else if (v.canConvert<QVariantHash>())
            adaptors.push_back(new AssociativePropertyAdaptor(parent));
    }
#endif

    // TODO external factories

    if (adaptors.isEmpty())
        return 0;
    if (adaptors.size() == 1) {
        adaptors.first()->setObject(oi);
        return adaptors.first();
    }

    auto aggregator = new PropertyAggregator(parent);
    std::for_each(adaptors.constBegin(), adaptors.constEnd(), [&aggregator](PropertyAdaptor *pa) {
        aggregator->addPropertyAdaptor(pa);
    });
    aggregator->setObject(oi);

    return aggregator;
}
