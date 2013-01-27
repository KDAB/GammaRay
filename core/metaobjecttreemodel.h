/*
  metaobjecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_METAOBJECTTREEMODEL_H
#define GAMMARAY_METAOBJECTTREEMODEL_H

#include <QModelIndex>
#include <QReadWriteLock>
#include <QVector>

namespace GammaRay {

class MetaObjectInfoTracker;

class MetaObjectTreeModel : public QAbstractItemModel
{
  Q_OBJECT

  public:
    enum Role {
      MetaObjectRole = Qt::UserRole + 1
    };

    enum Column {
      ObjectColumn,
      ObjectSelfCountColumn,
      ObjectInclusiveCountColumn,
      _Last
    };

    explicit MetaObjectTreeModel(QObject *parent = 0);
    virtual ~MetaObjectTreeModel();

    // reimplemented methods
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    // headers
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    // Probe callbacks
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

  private Q_SLOTS:
    void objectAddedMainThread(QObject *obj);
    void objectRemovedMainThread(const QMetaObject *metaObject);

  private:
    void addMetaObject(const QMetaObject *metaObject);
    void removeMetaObject(const QMetaObject *metaObject);

    QModelIndex indexForMetaObject(const QMetaObject *metaObject) const;
    const QMetaObject *metaObjectForIndex(const QModelIndex &index) const;

    mutable QReadWriteLock m_lock;

    // data
    QHash<const QMetaObject*, const QMetaObject*> m_childParentMap;
    QHash<const QMetaObject*, QVector<const QMetaObject*> > m_parentChildMap;

    MetaObjectInfoTracker* m_infoTracker;
};

}

Q_DECLARE_METATYPE(const QMetaObject *)

#endif // GAMMARAY_METAOBJECTTREEMODEL_H
