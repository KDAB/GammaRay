/*
  propertycontrollerinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_PROPERTYCONTROLLERINTERFACE_H
#define GAMMARAY_PROPERTYCONTROLLERINTERFACE_H

#include <QObject>
#include <QStringList>

namespace GammaRay {
/** @brief Client/Server interface of the property editor. */
class PropertyControllerInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        QStringList availableExtensions READ availableExtensions WRITE setAvailableExtensions NOTIFY availableExtensionsChanged)
public:
    explicit PropertyControllerInterface(const QString &name, QObject *parent = nullptr);
    ~PropertyControllerInterface() override;

    QString name() const;

    QStringList availableExtensions() const;
    void setAvailableExtensions(const QStringList &availableExtensions);

signals:
    void availableExtensionsChanged();

private:
    QString m_name;
    QStringList m_availableExtensions;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::PropertyControllerInterface,
                    "com.kdab.GammaRay.PropertyControllerInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_PROPERTYCONTROLLERINTERFACE_H
