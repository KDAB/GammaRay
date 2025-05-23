/*
  remoteviewinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "remoteviewinterface.h"
#include "streamoperators.h"

#include <common/objectbroker.h>
#include <common/remoteviewframe.h>
#include <QWindow>

#include <private/qeventpoint_p.h>
#include <QPainterPath>
Q_DECLARE_METATYPE(QPainterPath) // needed for Qt5

using namespace GammaRay;
QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(RemoteViewInterface::RequestMode)

QDataStream &operator<<(QDataStream &s, GammaRay::RemoteViewInterface::TouchPointStates states)
{
    return s << ( int )states;
}

QDataStream &operator>>(QDataStream &s, GammaRay::RemoteViewInterface::TouchPointStates &states)
{
    int st;
    s >> st;
    states = RemoteViewInterface::TouchPointStates(st);
    return s;
}

// Not available in Qt6, has QPointingDevice::PointerType instead
QDataStream &operator<<(QDataStream &s, QPointingDeviceUniqueId id)
{
    return s << id.numericId();
}

QDataStream &operator>>(QDataStream &s, QPointingDeviceUniqueId &id)
{
    int devId {};
    s >> devId;
    id = QPointingDeviceUniqueId::fromNumericId(devId);
    return s;
}

QDataStream &operator<<(QDataStream &s, const QList<QTouchEvent::TouchPoint> &points)
{
    // The (int) is fkn important!
    s << ( int )points.size();

    for (const auto &p : points) {
        s << p.id();
        s << p.state();
        s << p.scenePosition();
        s << p.ellipseDiameters();
        s << p.position();
        s << p.uniqueId();

        s << p.globalGrabPosition();
        s << p.globalLastPosition();
        s << p.globalPressPosition();
        s << p.globalPosition();

        //         s << p.velocity();
        s << p.pressure();
        s << p.rotation();
        s << ( quint64 )p.pressTimestamp();
        s << ( quint64 )p.timestamp();
    }
    return s;
}

template<class T>
void setPointValue(QDataStream &s, QEventPoint &p, void (*func)(QEventPoint &, T))
{
    typename std::decay<T>::type value;
    s >> value;
    (func)(p, value);
}

QDataStream &operator>>(QDataStream &s, QList<QTouchEvent::TouchPoint> &points)
{
    int count;
    s >> count;
    points.reserve(count);

    for (int i = 0; i < count; ++i) {
        QEventPoint p;

        setPointValue(s, p, &QMutableEventPoint::setId);
        setPointValue(s, p, &QMutableEventPoint::setState);
        setPointValue(s, p, &QMutableEventPoint::setScenePosition);
        setPointValue(s, p, &QMutableEventPoint::setEllipseDiameters);
        setPointValue(s, p, &QMutableEventPoint::setPosition);
        setPointValue(s, p, &QMutableEventPoint::setUniqueId);

        setPointValue(s, p, &QMutableEventPoint::setGlobalGrabPosition);
        setPointValue(s, p, &QMutableEventPoint::setGlobalLastPosition);
        setPointValue(s, p, &QMutableEventPoint::setGlobalPressPosition);
        setPointValue(s, p, &QMutableEventPoint::setGlobalPosition);

        //         setPointValue(s, p, &QMutableEventPoint::setVelocity);
        setPointValue(s, p, &QMutableEventPoint::setPressure);
        setPointValue(s, p, &QMutableEventPoint::setRotation);
        quint64 v;

        s >> v;
        QMutableEventPoint::setPressTimestamp(p, v);

        s >> v;
        QMutableEventPoint::setTimestamp(p, v);

        points.append(p);
    }
    return s;
}

QT_END_NAMESPACE

RemoteViewInterface::RemoteViewInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    ObjectBroker::registerObject(name, this);

    qRegisterMetaType<QPainterPath>(); // for QGraphicsItem::shape
    qRegisterMetaType<QTouchEvent::TouchPoint>();
    qRegisterMetaType<QList<QTouchEvent::TouchPoint>>();
    qRegisterMetaType<RemoteViewInterface::TouchPointStates>();

    StreamOperators::registerOperators<RequestMode>();
    StreamOperators::registerOperators<GammaRay::RemoteViewFrame>();
    StreamOperators::registerOperators<RemoteViewInterface::TouchPointStates>();
    StreamOperators::registerOperators<QList<QTouchEvent::TouchPoint>>();
    StreamOperators::registerOperators<QPointingDevice::PointerType>();
}

QString RemoteViewInterface::name() const
{
    return m_name;
}
