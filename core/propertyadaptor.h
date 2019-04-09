/*
  propertyadaptor.h

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

#ifndef GAMMARAY_PROPERTYADAPTOR_H
#define GAMMARAY_PROPERTYADAPTOR_H

#include "gammaray_core_export.h"
#include "objectinstance.h"

#include <QObject>
#include <QVector>

namespace GammaRay {
class PropertyData;

/** Generic interface for accessing properties from various sources of an object. */
class GAMMARAY_CORE_EXPORT PropertyAdaptor : public QObject
{
    Q_OBJECT
public:
    explicit PropertyAdaptor(QObject *parent = nullptr);
    ~PropertyAdaptor() override;

    /** Returns the object instance who's properties this accesses. */
    const ObjectInstance &object() const;
    /** Set the object instance who's properties we want to access. */
    void setObject(const ObjectInstance &oi);

    /** Number of properties. */
    virtual int count() const = 0;

    /** Property data for all properties. */
    virtual PropertyData propertyData(int index) const = 0;

    /** Write a single property value. */
    virtual void writeProperty(int index, const QVariant &value);

    /** Returns true if this adaptor allows adding new properties. */
    virtual bool canAddProperty() const;

    /** Adds a new property. */
    virtual void addProperty(const PropertyData &data);

    /** Resets the specified property. */
    virtual void resetProperty(int index);

    /** Property adaptor of parent object instance, if any. */
    PropertyAdaptor *parentAdaptor() const;

signals:
    void propertyAdded(int first, int last);
    void propertyRemoved(int first, int last);
    void propertyChanged(int first, int last);

    /** Emit this when you noticed that the object who's properties we are
     * looking at became invalid.
     */
    void objectInvalidated();

protected:
    virtual void doSetObject(const ObjectInstance &oi);

private:
    friend class PropertyAggregator;
    ObjectInstance m_oi;
};
}

Q_DECLARE_METATYPE(GammaRay::PropertyAdaptor *)

#endif // GAMMARAY_PROPERTYADAPTOR_H
