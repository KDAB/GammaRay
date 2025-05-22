/*
  remoteviewclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_REMOTEVIEWCLIENT_H
#define GAMMARAY_REMOTEVIEWCLIENT_H

#include <common/remoteviewinterface.h>

namespace GammaRay {
class RemoteViewClient : public RemoteViewInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::RemoteViewInterface)
public:
    explicit RemoteViewClient(const QString &name, QObject *parent = nullptr);
    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode) override;
    void pickElementId(const GammaRay::ObjectId &id) override;
    void sendKeyEvent(int type, int key, int modifiers,
                      const QString &text = QString(), bool autorep = false,
                      ushort count = 1) override;
    void sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                        int modifiers) override;
    void sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta, int buttons,
                        int modifiers) override;
    void sendTouchEvent(const QString &deviceName, int deviceSystemId, int type, int touchDeviceType, int deviceCaps, int touchDeviceMaxTouchPoints, int modifiers,
                        const QList<QTouchEvent::TouchPoint> &touchPoints) override;
    void setViewActive(bool active) override;
    void sendUserViewport(const QRectF &userViewport) override;
    void clientViewUpdated() override;
    void requestCompleteFrame() override;
};
}

#endif // GAMMARAY_REMOTEVIEWCLIENT_H
