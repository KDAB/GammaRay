/*
  quickinspectorinterface.h

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

#ifndef GAMMARAY_QUICKINSPECTORINTERFACE_H
#define GAMMARAY_QUICKINSPECTORINTERFACE_H

#include <QObject>
#include <QRectF>
#include <QVariantMap>
#include <QEvent>

class QImage;

namespace GammaRay {

class QuickInspectorInterface : public QObject
{
  Q_OBJECT
public:
  explicit QuickInspectorInterface(QObject *parent = 0);
  ~QuickInspectorInterface();

public slots:
  virtual void selectWindow(int index) = 0;
  /// Ask for a new screenshot of the scene.
  virtual void renderScene() = 0;
  virtual void sendKeyEvent(int type, int key, int modifiers, const QString & text = QString(), bool autorep = false, ushort count = 1) = 0;
  virtual void sendMouseEvent(int type, const QPointF & localPos, int button, int buttons, int modifiers) = 0;

signals:
  /// Emitted when the view has been newly rendered, for the client to request an update.
  void sceneChanged();
  void sceneRendered(const QImage &img, QVariantMap geometryData);

};
}

Q_DECLARE_INTERFACE(GammaRay::QuickInspectorInterface, "com.kdab.GammaRay.QuickInspectorInterface/1.0")

#endif // GAMMARAY_QUICKINSPECTORINTERFACE_H
