/*
  remoteviewserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_REMOTEVIEWSERVER_H
#define GAMMARAY_REMOTEVIEWSERVER_H

#include <memory>

#include "gammaray_core_export.h"

#include <common/remoteviewinterface.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTimer;
class QWindow;
class QTouchDevice;
class QPointingDevice;
QT_END_NAMESPACE

namespace GammaRay {
/** Server part of the remote view widget. */
class GAMMARAY_CORE_EXPORT RemoteViewServer : public RemoteViewInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::RemoteViewInterface)
public:
    explicit RemoteViewServer(const QString &name, QObject *parent = nullptr);

    using EventReceiver = QWindow;
    /// event receiver for input redirection
    void setEventReceiver(EventReceiver *receiver);

    /// resets the client view if the window selection changed
    void resetView();

    /// returns @c true if there is a client displaying our content
    bool isActive() const;

    /// set the grabber ready state
    void setGrabberReady(bool ready);

    /// sends a new frame to the client
    void sendFrame(const RemoteViewFrame &frame);

    QRectF userViewport() const;

public slots:
    /// call this to indicate the source has changed and the client requires an update
    void sourceChanged();
    void requestCompleteFrame() override;

signals:
    void elementsAtRequested(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void doPickElementId(const GammaRay::ObjectId &id);
    /// when receiving this signal, obtain a new frame and send it to the client
    void requestUpdate();

private:
    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode) override;
    void pickElementId(const GammaRay::ObjectId &id) override;
    void sendKeyEvent(int type, int key, int modifiers, const QString &text, bool autorep,
                      ushort count) override;
    void sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                        int modifiers) override;
    void sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta, int buttons,
                        int modifiers) override;
    void sendTouchEvent(const QString &deviceName, int deviceSystemId, int type, int touchDeviceType, int deviceCaps, int touchDeviceMaxTouchPoints, int modifiers,
                        const QList<QTouchEvent::TouchPoint> &touchPoints) override;
    void setViewActive(bool active) override;
    void sendUserViewport(const QRectF &userViewport) override;
    void clientViewUpdated() override;

    void checkRequestUpdate();

private slots:
    void clientConnectedChanged(bool connected);
    void requestUpdateTimeout();

private:
    QPointer<EventReceiver> m_eventReceiver;
    QTimer *m_updateTimer;
    QRectF m_lastTransmittedViewRect;
    QRectF m_lastTransmittedImageRect;
    QRectF m_userViewport;
    bool m_clientActive;
    bool m_sourceChanged;
    bool m_clientReady;
    bool m_grabberReady;
    bool m_pendingReset;
    bool m_pendingCompleteFrame;
    std::unique_ptr<QPointingDevice> m_touchDevice;
};
}

#endif // GAMMARAY_REMOTEVIEWSERVER_H
