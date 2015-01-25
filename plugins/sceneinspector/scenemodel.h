/*
  scenemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H
#define GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H

#include <QAbstractItemModel>
#include <common/modelroles.h>

class QGraphicsScene;
class QGraphicsItem;

namespace GammaRay {

class SceneModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    enum Role {
      SceneItemRole = UserRole + 1
    };
    explicit SceneModel(QObject *parent = 0);
    void setScene(QGraphicsScene *scene);
    QGraphicsScene *scene() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private:
    QList<QGraphicsItem*> topLevelItems() const;
    /// Returns a string type name for the given QGV item type id
    QString typeName(int itemType) const;

    QGraphicsScene *m_scene;
    QHash<int, QString> m_typeNames;
};

}

#endif // GAMMARAY_SCENEMODEL_H
