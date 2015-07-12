/*
  dynamicpropertyadaptor.h

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

#ifndef GAMMARAY_DYNAMICPROPERTYADAPTOR_H
#define GAMMARAY_DYNAMICPROPERTYADAPTOR_H

#include "propertyadaptor.h"

namespace GammaRay {

/** Access to dynamic QObject properties. */
class DynamicPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit DynamicPropertyAdaptor(QObject* parent = 0);
    ~DynamicPropertyAdaptor();

    void setObject(const ObjectInstance& oi) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;
    PropertyData propertyData(int index) const Q_DECL_OVERRIDE;
    void writeProperty(int index, const QVariant& value) Q_DECL_OVERRIDE;
    bool canAddProperty() const Q_DECL_OVERRIDE;
    void addProperty(const PropertyData& data) Q_DECL_OVERRIDE;

protected:
    bool eventFilter(QObject *receiver, QEvent *event) Q_DECL_OVERRIDE;

private:
    QObject *m_obj;
    QList<QByteArray> m_propNames;
};

}

#endif // GAMMARAY_DYNAMICPROPERTYADAPTOR_H
