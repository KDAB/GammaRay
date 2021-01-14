/*
  scenemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H
#define GAMMARAY_SCENEINSPECTOR_SCENEMODEL_H

#include <QAbstractItemModel>
#include <common/modelroles.h>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsItem;
QT_END_NAMESPACE

namespace GammaRay {
class SceneModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role {
        SceneItemRole = UserRole + 1
    };
    explicit SceneModel(QObject *parent = nullptr);
    void setScene(QGraphicsScene *scene);
    QGraphicsScene *scene() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QList<QGraphicsItem *> topLevelItems() const;
    /// Returns a string type name for the given QGV item type id
    QString typeName(int itemType) const;

    QGraphicsScene *m_scene;
    QHash<int, QString> m_typeNames;
};
}

#endif // GAMMARAY_SCENEMODEL_H
