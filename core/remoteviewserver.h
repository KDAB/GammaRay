/*
  remoteviewserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_REMOTEVIEWSERVER_H
#define GAMMARAY_REMOTEVIEWSERVER_H

#include "gammaray_core_export.h"

#include <common/remoteviewinterface.h>

QT_BEGIN_NAMESPACE
class QTimer;
class QWindow;
QT_END_NAMESPACE

namespace GammaRay {
/** Server part of the remote view widget. */
class GAMMARAY_CORE_EXPORT RemoteViewServer : public RemoteViewInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::RemoteViewInterface)
public:
    explicit RemoteViewServer(const QString &name, QObject *parent = Q_NULLPTR);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    typedef QWindow EventReceiver;
#else
    typedef QObject EventReceiver;
#endif
    /// event receiver for input redirection
    void setEventReceiver(EventReceiver *receiver);

    /// resets the client view if the window selection changed
    void resetView();

    /// returns @c true if there is a client displaying our content
    bool isActive() const;

    /// sends a new frame to the client
    void sendFrame(const RemoteViewFrame &frame);

public slots:
    /// call this to indicate the source has changed and the client requires an update
    void sourceChanged();

signals:
    void elementsAtRequested(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void doPickElementId(const GammaRay::ObjectId &id);
    /// when receiving this signal, obtain a new frame and send it to the client
    void requestUpdate();

private:
    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode) Q_DECL_OVERRIDE;
    void pickElementId(const GammaRay::ObjectId &id) Q_DECL_OVERRIDE;
    void sendKeyEvent(int type, int key, int modifiers, const QString &text, bool autorep,
                      ushort count) Q_DECL_OVERRIDE;
    void sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                        int modifiers) Q_DECL_OVERRIDE;
    void sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta, int buttons,
                        int modifiers) Q_DECL_OVERRIDE;
    void setViewActive(bool active) Q_DECL_OVERRIDE;
    void clientViewUpdated() Q_DECL_OVERRIDE;

    void checkRequestUpdate();

private slots:
    void clientConnectedChanged(bool connected);
    void requestUpdateTimeout();

private:
    EventReceiver *m_eventReceiver;
    QTimer *m_updateTimer;
    bool m_clientActive;
    bool m_sourceChanged;
    bool m_clientReady;
};
}

#endif // GAMMARAY_REMOTEVIEWSERVER_H
