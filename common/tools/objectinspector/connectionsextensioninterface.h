/*
  connectionsextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H
#define GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {

/** Communication interface for the connections tab in the property view. */
class ConnectionsExtensionInterface : public QObject
{
  Q_OBJECT
  public:
    explicit ConnectionsExtensionInterface(const QString &name, QObject *parent = 0);
    ~ConnectionsExtensionInterface();

    QString name() const;

  public slots:
    virtual void navigateToSender(int modelRow) = 0;
    virtual void navigateToReceiver(int modelRow) = 0;

  private:
    QString m_name;
};

}

Q_DECLARE_INTERFACE(GammaRay::ConnectionsExtensionInterface, "com.kdab.GammaRay.ConnectionsExtensionInterface")

#endif // GAMMARAY_CONNECTIONSEXTENSIONINTERFACE_H
