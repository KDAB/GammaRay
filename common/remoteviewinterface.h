/*
  remoteviewinterface.h

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

#ifndef GAMMARAY_REMOTEVIEWINTERFACE_H
#define GAMMARAY_REMOTEVIEWINTERFACE_H

#include "gammaray_common_export.h"

#include "objectid.h"

#include <QObject>
#include <QPoint>

namespace GammaRay {
class RemoteViewFrame;

/** Communication interface for the remote view widget. */
class GAMMARAY_COMMON_EXPORT RemoteViewInterface : public QObject
{
    Q_OBJECT
public:
    enum RequestMode {
        RequestBest,
        RequestAll
    };

    explicit RemoteViewInterface(const QString &name, QObject *parent = Q_NULLPTR);

    QString name() const;

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

    virtual void setViewActive(bool active) = 0;

    /// Tell the server we are ready for the next frame.
    virtual void clientViewUpdated() = 0;

signals:
    void reset();
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);
    void frameUpdated(const GammaRay::RemoteViewFrame &frame);

private:
    QString m_name;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::RemoteViewInterface::RequestMode)
Q_DECLARE_INTERFACE(GammaRay::RemoteViewInterface, "com.kdab.GammaRay.RemoteViewInterface/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_REMOTEVIEWINTERFACE_H
