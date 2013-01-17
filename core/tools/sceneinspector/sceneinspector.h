/*
  sceneinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEINSPECTOR_H
#define GAMMARAY_SCENEINSPECTOR_SCENEINSPECTOR_H

#include "include/toolfactory.h"

#include <QGraphicsScene>
#include <QWidget>

class QModelIndex;

namespace GammaRay {

class SceneModel;

namespace Ui {
  class SceneInspector;
}

class SceneInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit SceneInspector(ProbeInterface *probe, QWidget *parent = 0);

  private slots:
    void sceneSelected(int index);
    void sceneItemSelected(const QModelIndex &index);
    void sceneItemSelected(QGraphicsItem *item);
    void widgetSelected(QWidget *item, const QPoint &pos);

  private:
    QString findBestType(QGraphicsItem *item);

  private:
    QScopedPointer<Ui::SceneInspector> ui;
    SceneModel *m_sceneModel;
};

class SceneInspectorFactory : public QObject,
                              public StandardToolFactory<QGraphicsScene, SceneInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit SceneInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Graphics Scenes");
    }
};

}

#endif // GAMMARAY_SCENEINSPECTOR_H
