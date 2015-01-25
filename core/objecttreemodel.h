/*
  objecttreemodel.h

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

#ifndef GAMMARAY_OBJECTTREEMODEL_H
#define GAMMARAY_OBJECTTREEMODEL_H

#include "objectmodelbase.h"

#include <QVector>

namespace GammaRay {

class Probe;

class ObjectTreeModel : public ObjectModelBase<QAbstractItemModel>
{
  Q_OBJECT
  public:
    explicit ObjectTreeModel(Probe *probe);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

  private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectReparented(QObject *obj);

  private:
    QModelIndex indexForObject(QObject *object) const;

  private:
    QHash<QObject*, QObject*> m_childParentMap;
    QHash<QObject*, QVector<QObject*> > m_parentChildMap;
};

}

#endif // GAMMARAY_OBJECTTREEMODEL_H
