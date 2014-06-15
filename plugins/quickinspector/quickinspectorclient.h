/*
  quickinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORCLIENT_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORCLIENT_H

#include "quickinspectorinterface.h"

class QEvent;

namespace GammaRay {

class QuickInspectorClient : public QuickInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::QuickInspectorInterface)

public:
  explicit QuickInspectorClient(QObject *parent = 0);
  ~QuickInspectorClient();

public slots:
  void selectWindow(int index);
  void renderScene();

  void sendKeyEvent(int type, int key, int modifiers, const QString &text,
                    bool autorep, ushort count);

  void sendMouseEvent(int type, const QPointF &localPos,
                      int button, int buttons, int modifiers);

  void sendWheelEvent(const QPointF &localPos, QPoint pixelDelta,
                      QPoint angleDelta, int buttons, int modifiers);

  void setCustomRenderMode(GammaRay::QuickInspectorInterface::RenderMode customRenderMode);

  void checkFeatures();
};
}

#endif // GAMMARAY_QUICKINSPECTORCLIENT_H
