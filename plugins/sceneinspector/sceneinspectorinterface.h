/*
  sceneinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_SCENEINSPECTORINTERFACE_H
#define GAMMARAY_SCENEINSPECTORINTERFACE_H

#include <QObject>

class QSize;
class QTransform;
class QRectF;
class QPixmap;

namespace GammaRay {

class SceneInspectorInterface : public QObject
{
  Q_OBJECT
  public:
    explicit SceneInspectorInterface(QObject *parent = 0);
    virtual ~SceneInspectorInterface();

    virtual void initializeGui() = 0;

  public slots:
    virtual void renderScene(const QTransform &transform, const QSize &size) = 0;

  signals:
    void sceneRectChanged(const QRectF &rect);
    void sceneChanged();
    void sceneRendered(const QPixmap &view);
};

}

Q_DECLARE_INTERFACE(GammaRay::SceneInspectorInterface, "com.kdab.GammaRay.SceneInspector")

#endif // GAMMARAY_SCENEINSPECTORINTERFACE_H
