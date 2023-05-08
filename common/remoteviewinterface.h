/*
  remoteviewinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_REMOTEVIEWINTERFACE_H
#define GAMMARAY_REMOTEVIEWINTERFACE_H

#include "gammaray_common_export.h"

#include "objectid.h"

#include <QObject>
#include <QPoint>
#include <QTouchEvent>

namespace GammaRay {
class RemoteViewFrame;

/** Communication interface for the remote view widget. */
class GAMMARAY_COMMON_EXPORT RemoteViewInterface : public QObject
{
    Q_OBJECT
public:
    enum RequestMode
    {
        RequestBest,
        RequestAll
    };

    explicit RemoteViewInterface(const QString &name, QObject *parent = nullptr);

    QString name() const;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using TouchPointStates = QEventPoint::States;
#else
    using TouchPointStates = Qt::TouchPointStates;
#endif

public slots:
    virtual void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode) = 0;
    virtual void pickElementId(const GammaRay::ObjectId &id) = 0;

    virtual void sendKeyEvent(int type, int key, int modifiers,
                              const QString &text = QString(), bool autorep = false,
                              ushort count = 1) = 0;

    virtual void sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                                int modifiers) = 0;

    virtual void sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta,
                                int buttons, int modifiers) = 0;

    virtual void sendTouchEvent(int type, int touchDeviceType, int deviceCaps, int touchDeviceMaxTouchPoints, int modifiers,
                                int touchPointStates,
                                const QList<QTouchEvent::TouchPoint> &touchPoints) = 0;

    virtual void sendUserViewport(const QRectF &userViewport) = 0;

    virtual void setViewActive(bool active) = 0;

    /// Tell the server we are ready for the next frame.
    virtual void clientViewUpdated() = 0;

    virtual void requestCompleteFrame() = 0;

signals:
    void reset();
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    void elementsAtReceived(const QList<GammaRay::ObjectId> &ids, int bestCandidate);
#else
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);
#endif
    void frameUpdated(const GammaRay::RemoteViewFrame &frame);

private:
    QString m_name;
};

}

Q_DECLARE_METATYPE(QTouchEvent::TouchPoint)
Q_DECLARE_METATYPE(GammaRay::RemoteViewInterface::TouchPointStates)
Q_DECLARE_METATYPE(QList<QTouchEvent::TouchPoint>)
Q_DECLARE_METATYPE(GammaRay::RemoteViewInterface::RequestMode)

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
Q_DECLARE_METATYPE(QPointingDevice::PointerType)
Q_DECLARE_METATYPE(QPointingDeviceUniqueId)
#else
Q_DECLARE_METATYPE(QTouchEvent::TouchPoint::InfoFlags)
#endif

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::RemoteViewInterface, "com.kdab.GammaRay.RemoteViewInterface/1.0")
QT_END_NAMESPACE

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &s, const QList<QTouchEvent::TouchPoint> &points);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &s, QList<QTouchEvent::TouchPoint> &points);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &s, GammaRay::RemoteViewInterface::TouchPointStates &states);
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &s, GammaRay::RemoteViewInterface::TouchPointStates states);
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &s, QPointingDeviceUniqueId id);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &s, QPointingDeviceUniqueId &id);
#endif

#endif // GAMMARAY_REMOTEVIEWINTERFACE_H
