/*
  connectionsextension.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_OBJECTINSPECTOR_CONNECTIONSEXTENSION_H
#define GAMMARAY_OBJECTINSPECTOR_CONNECTIONSEXTENSION_H

#include "common/tools/objectinspector/connectionsextensioninterface.h"
#include "core/propertycontrollerextension.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#define USE_QT_CONNECTIONS_LIST
#endif

namespace GammaRay {

class ConnectionFilterProxyModel;
class InboundConnectionsModel;
class OutboundConnectionsModel;

class ConnectionsExtension : public ConnectionsExtensionInterface,
                             public PropertyControllerExtension
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ConnectionsExtensionInterface)
  public:
    explicit ConnectionsExtension(PropertyController *controller);
    ~ConnectionsExtension();

    bool setQObject(QObject *object);

  public slots:
    void navigateToReceiver(int modelRow);
    void navigateToSender(int modelRow);

  private:
#ifndef USE_QT_CONNECTIONS_LIST
    ConnectionFilterProxyModel *m_inboundModel;
    ConnectionFilterProxyModel *m_outboundModel;
#else
    InboundConnectionsModel *m_inboundModel;
    OutboundConnectionsModel *m_outboundModel;
#endif
};

}

#endif // GAMMARAY_CONNECTIONSEXTENSION_H
