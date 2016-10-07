/*
  metaobjecttreemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_METAOBJECTTREEMODEL_H
#define GAMMARAY_METAOBJECTTREEMODEL_H

#include <QModelIndex>
#include <QSet>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class Probe;

class MetaObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit MetaObjectTreeModel(QObject *parent = Q_NULLPTR);
    ~MetaObjectTreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits,
                          Qt::MatchFlags flags) const Q_DECL_OVERRIDE;

    void scanMetaTypes();

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

private:
    void addMetaObject(const QMetaObject *metaObject);
    void removeMetaObject(const QMetaObject *metaObject);

    bool isKnownMetaObject(const QMetaObject *metaObject) const;
    QModelIndex indexForMetaObject(const QMetaObject *metaObject) const;
    const QMetaObject *metaObjectForIndex(const QModelIndex &index) const;

    void scheduleDataChange(const QMetaObject *mo);

private slots:
    void emitPendingDataChanged();

private:
    QHash<const QMetaObject *, const QMetaObject *> m_childParentMap;
    QHash<const QMetaObject *, QVector<const QMetaObject *> > m_parentChildMap;

    struct MetaObjectInfo
    {
        MetaObjectInfo()
            : selfCount(0)
            , selfAliveCount(0)
            , inclusiveCount(0)
            , inclusiveAliveCount(0) {}

        /// Number of objects of a particular meta object type ever created
        int selfCount;
        /// Number of instances of a meta object currently alive
        int selfAliveCount;
        /**
         * Number of objects of the exact meta object type
         * + number of objects of type that inherit from this meta type
         */
        int inclusiveCount;
        /// Inclusive instance count currently alive
        int inclusiveAliveCount;
    };
    QHash<const QMetaObject*, MetaObjectInfo> m_metaObjectInfoMap;
    /// meta objects at creation time, so we can correctly decrement instance counts
    /// after destruction
    QHash<QObject*, const QMetaObject*> m_metaObjectMap;

    QSet<const QMetaObject *> m_pendingDataChanged;
    QTimer *m_pendingDataChangedTimer;
};
}

#endif // GAMMARAY_METAOBJECTTREEMODEL_H
