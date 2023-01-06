/*
  propertysyncer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    struct ObjectInfo
    {
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
