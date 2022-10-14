/*
  remoteviewinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remoteviewinterface.h"
#include "streamoperators.h"

#include <common/objectbroker.h>
#include <common/remoteviewframe.h>
#include <QWindow>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qeventpoint_p.h>
#endif

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QDataStream &operator<<(QDataStream &s, QTouchEvent::TouchPoint::InfoFlags flags)
{
    return s << ( int )flags;
}

QDataStream &operator>>(QDataStream &s, QTouchEvent::TouchPoint::InfoFlags &flags)
{
    int f;
    s >> f;
    flags = QTouchEvent::TouchPoint::InfoFlags(f);
    return s;
}
#else
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
#endif

QDataStream &operator<<(QDataStream &s, const QList<QTouchEvent::TouchPoint> &points)
{
    // The (int) is fkn important!
    s << ( int )points.count();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
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
#else
    for (const auto &p : points) {
        s << p.id();
        s << p.state();
        s << p.rect() << p.sceneRect() << p.screenRect();
        s << p.normalizedPos();
        s << p.startPos() << p.startScenePos() << p.startScreenPos() << p.startNormalizedPos();
        s << p.lastPos() << p.lastScenePos() << p.lastScreenPos() << p.lastNormalizedPos();
        s << p.pressure();
        s << p.velocity();
        s << p.flags();
        s << p.rawScreenPositions();
    }
#endif
    return s;
}

#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
template<class T, class TouchPoint>
void setPointValue(QDataStream &s, TouchPoint &p, void (TouchPoint::*func)(T))
{
    typename std::decay<T>::type value;
    s >> value;
    (p.*func)(value);
}
#else
template<class T>
void setPointValue(QDataStream &s, QEventPoint &p, void (*func)(QEventPoint &, T))
{
    typename std::decay<T>::type value;
    s >> value;
    (func)(p, value);
}
#endif

QDataStream &operator>>(QDataStream &s, QList<QTouchEvent::TouchPoint> &points)
{
    int count;
    s >> count;
    points.reserve(count);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    for (int i = 0; i < count; ++i) {
#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
        QMutableEventPoint p;
#else
        QEventPoint p;
#endif

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
#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
        p.setPressTimestamp(v);
#else
        QMutableEventPoint::setPressTimestamp(p, v);
#endif

        s >> v;
#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 4)
        p.setTimestamp(v);
#else
        QMutableEventPoint::setTimestamp(p, v);
#endif

        points.append(p);
    }
#else
    for (int i = 0; i < count; ++i) {
        QTouchEvent::TouchPoint p;

        setPointValue(s, p, &QTouchEvent::TouchPoint::setId);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setState);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setRect);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setSceneRect);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setScreenRect);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setNormalizedPos);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setStartPos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setStartScenePos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setStartScreenPos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setStartNormalizedPos);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setLastPos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setLastScenePos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setLastScreenPos);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setLastNormalizedPos);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setPressure);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setVelocity);

        setPointValue(s, p, &QTouchEvent::TouchPoint::setFlags);
        setPointValue(s, p, &QTouchEvent::TouchPoint::setRawScreenPositions);

        points.append(p);
    }
#endif
    return s;
}

QT_END_NAMESPACE


RemoteViewInterface::RemoteViewInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    ObjectBroker::registerObject(name, this);

    qRegisterMetaType<QTouchEvent::TouchPoint>();
    qRegisterMetaType<QList<QTouchEvent::TouchPoint>>();
    qRegisterMetaType<RemoteViewInterface::TouchPointStates>();

    StreamOperators::registerOperators<RequestMode>();
    StreamOperators::registerOperators<GammaRay::RemoteViewFrame>();
    StreamOperators::registerOperators<RemoteViewInterface::TouchPointStates>();
    StreamOperators::registerOperators<QList<QTouchEvent::TouchPoint>>();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    StreamOperators::registerOperators<QPointingDevice::PointerType>();
#else
    StreamOperators::registerOperators<QTouchEvent::TouchPoint::InfoFlags>();
#endif
}

QString RemoteViewInterface::name() const
{
    return m_name;
}
