/*
  networkobject.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_NETWORKOBJECT_H
#define GAMMARAY_NETWORKOBJECT_H

#include "include/gammaray_common_export.h"
#include "protocol.h"

#include <QObject>

namespace GammaRay {

class Message;

/** Base class for objects that forwards signals bidirectionally over the network.
 *  Do not create directly, instead retrieve instances via GammaRay::ObjectBroker.
 *  TODO: add support for signal arguments
 */
class GAMMARAY_COMMON_EXPORT NetworkObject : public QObject
{
  Q_OBJECT
public:
  explicit NetworkObject(const QString &objectName, QObject* parent);
  ~NetworkObject();

  /** Invoke methods connected to @p signalName on the other end. */
  void emitSignal(const char* signalName, const QVariantList& args = QVariantList());

  // TODO publish signals

  /** @p slot on @p receiver will be called whenever the remote object ends @p singnalName. */
  void subscribeToSignal(const char *signalName, QObject* receiver, const char* slot);

protected:
  QString m_objectName;
  Protocol::ObjectAddress m_myAddress;

private:
  void emitLocal(const char *signalName, const QVariantList &args);

private slots:
  void newMessage(const GammaRay::Message &msg);

private:
  QHash<QByteArray, QPair<QObject*, QByteArray> > m_subscriptions;
};

}

#endif // GAMMARAY_NETWORKOBJECT_H
