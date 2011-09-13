/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef KIDENTITYPROXYMODEL_H
#define KIDENTITYPROXYMODEL_H

#include <QtGui/QAbstractProxyModel>

#include "kdeui_export.h"

class KIdentityProxyModelPrivate;

class KDEUI_EXPORT KIdentityProxyModel : public QAbstractProxyModel
{
  Q_OBJECT
public:
  explicit KIdentityProxyModel(QObject* parent = 0);
  virtual ~KIdentityProxyModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
  QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
  QModelIndex parent(const QModelIndex& child) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

  QItemSelection mapSelectionFromSource(const QItemSelection& selection) const;
  QItemSelection mapSelectionToSource(const QItemSelection& selection) const;
  QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
  void setSourceModel(QAbstractItemModel* sourceModel);

  bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
  bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

  virtual bool canFetchMore(const QModelIndex& parent) const;
  virtual void fetchMore(const QModelIndex& parent);
  virtual QStringList mimeTypes() const;
  virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
  virtual Qt::DropActions supportedDropActions() const;

protected:
  KIdentityProxyModel(KIdentityProxyModelPrivate *privateClass, QObject* parent);
  KIdentityProxyModelPrivate * const d_ptr;

protected Q_SLOTS:
  virtual void resetInternalData();

private:
  Q_DECLARE_PRIVATE(KIdentityProxyModel)
  Q_DISABLE_COPY(KIdentityProxyModel)

  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsAboutToBeInserted(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsInserted(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsAboutToBeRemoved(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsRemoved(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceRowsMoved(QModelIndex,int,int,QModelIndex,int))

  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsAboutToBeInserted(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsInserted(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsAboutToBeRemoved(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsRemoved(QModelIndex,int,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceColumnsMoved(QModelIndex,int,int,QModelIndex,int))

  Q_PRIVATE_SLOT(d_func(), void _k_sourceDataChanged(QModelIndex,QModelIndex))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last))

  Q_PRIVATE_SLOT(d_func(), void _k_sourceLayoutAboutToBeChanged())
  Q_PRIVATE_SLOT(d_func(), void _k_sourceLayoutChanged())
  Q_PRIVATE_SLOT(d_func(), void _k_sourceChildrenLayoutsAboutToBeChanged(const QModelIndex &parent1, const QModelIndex &parent2))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceChildrenLayoutsChanged(const QModelIndex &parent1, const QModelIndex &parent2))
  Q_PRIVATE_SLOT(d_func(), void _k_sourceModelAboutToBeReset())
  Q_PRIVATE_SLOT(d_func(), void _k_sourceModelReset())
  Q_PRIVATE_SLOT(d_func(), void _k_sourceModelDestroyed())
};

#endif // KIDENTITYPROXYMODEL_H
