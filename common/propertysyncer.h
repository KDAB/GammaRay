/*
  propertysyncer.h

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

#ifndef GAMMARAY_PROPERTYSYNCER_H
#define GAMMARAY_PROPERTYSYNCER_H

#include "gammaray_common_export.h"

#include <common/protocol.h>

#include <QObject>
#include <QVector>

namespace GammaRay {
class Message;

/** Infrastructure for syncing property values between a local and a remote object. */
class GAMMARAY_COMMON_EXPORT PropertySyncer : public QObject
{
    Q_OBJECT
public:
    explicit PropertySyncer(QObject *parent = nullptr);
    ~PropertySyncer() override;

    /** Add an object that should be monitored for to be synced property changes. */
    void addObject(Protocol::ObjectAddress addr, QObject *obj);

    /** Enable property syncing for the object with address @p addr.
     *  Use this to suspend property syncing for objects that aren't used on the client.
     *  Property syncing is disabled by default.
     */
    void setObjectEnabled(Protocol::ObjectAddress addr, bool enabled);

    /** Object address of the property syncer, for communicating with the other side. */
    Protocol::ObjectAddress address() const;
    void setAddress(Protocol::ObjectAddress addr);

    /** Request the initial property states when adding new objects.
     *  This is typically enabled on the client side.
     */
    void setRequestInitialSync(bool initialSync);

public slots:
    /** Feed in incoming network messages here. */
    void handleMessage(const GammaRay::Message &msg);

signals:
    /** Outgoing network messages, send those via Endpoint. */
    void message(const GammaRay::Message &msg);

private slots:
    void propertyChanged();
    void objectDestroyed(QObject *obj);

private:
    struct ObjectInfo {
        Protocol::ObjectAddress addr;
        QObject *obj;
        bool recursionLock;
        bool enabled;
    };
    QVector<ObjectInfo> m_objects;
    Protocol::ObjectAddress m_address;
    bool m_initialSync;
};
}

#endif // GAMMARAY_PROPERTYSYNCER_H
