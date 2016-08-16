/*
  remoteviewclient.h

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

#ifndef GAMMARAY_REMOTEVIEWCLIENT_H
#define GAMMARAY_REMOTEVIEWCLIENT_H

#include <common/remoteviewinterface.h>

namespace GammaRay {
class RemoteViewClient : public RemoteViewInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::RemoteViewInterface)
public:
    explicit RemoteViewClient(const QString &name, QObject *parent = Q_NULLPTR);
    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode) Q_DECL_OVERRIDE;
    void pickElementId(const GammaRay::ObjectId &id) Q_DECL_OVERRIDE;
    void sendKeyEvent(int type, int key, int modifiers,
                      const QString &text = QString(), bool autorep = false,
                      ushort count = 1) Q_DECL_OVERRIDE;
    void sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                        int modifiers) Q_DECL_OVERRIDE;
    void sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta, int buttons,
                        int modifiers) Q_DECL_OVERRIDE;
    void setViewActive(bool active) Q_DECL_OVERRIDE;
    void clientViewUpdated() Q_DECL_OVERRIDE;
};
}

#endif // GAMMARAY_REMOTEVIEWCLIENT_H
