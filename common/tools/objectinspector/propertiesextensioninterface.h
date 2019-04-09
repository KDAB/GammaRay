/*
  propertycontrollerinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H
#define GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {
/** @brief Client/Server interface of the property editor. */
class PropertiesExtensionInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        bool canAddProperty READ canAddProperty WRITE setCanAddProperty NOTIFY canAddPropertyChanged)
    Q_PROPERTY(
        bool hasPropertyValues READ hasPropertyValues WRITE setHasPropertyValues NOTIFY hasPropertyValuesChanged)
public:
    explicit PropertiesExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~PropertiesExtensionInterface() override;

    const QString &name() const;

    bool canAddProperty() const;
    void setCanAddProperty(bool canAdd);

    bool hasPropertyValues() const;
    void setHasPropertyValues(bool hasValues);

public slots:
    virtual void setProperty(const QString &name, const QVariant &value) = 0;

signals:
    void canAddPropertyChanged();
    void hasPropertyValuesChanged();

private:
    QString m_name;
    bool m_canAddProperty;
    bool m_hasPropertyValues;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::PropertiesExtensionInterface,
                    "com.kdab.GammaRay.PropertiesExtensionInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H
