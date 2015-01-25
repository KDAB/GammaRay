/*
  propertiesextensionclient.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_PROPERTIESEXTENSIONCLIENT_H
#define GAMMARAY_PROPERTIESEXTENSIONCLIENT_H

#include <common/tools/objectinspector/propertiesextensioninterface.h>

namespace GammaRay {

class PropertiesExtensionClient : public PropertiesExtensionInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::PropertiesExtensionInterface)
  public:
    explicit PropertiesExtensionClient(const QString &name, QObject *parent = 0);
    virtual ~PropertiesExtensionClient();

  public slots:
//     virtual void activateMethod();
//     virtual void invokeMethod(Qt::ConnectionType type);
    virtual void navigateToValue(int modelRow);
    virtual void setProperty(const QString& name, const QVariant& value);
    virtual void resetProperty(const QString& name);
};

}

#endif // GAMMARAY_PROPERTIESEXTENSIONCLIENT_H
