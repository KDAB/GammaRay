/*
  translatorsproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#include "translatorsproxymodel.h"

#include <common/objectmodel.h>
#include <core/util.h>

#include "translatorwrapper.h"

using namespace GammaRay;

TranslatorsProxyModel::TranslatorsProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}
void TranslatorsProxyModel::setSourceModel(QAbstractItemModel *model)
{
  QIdentityProxyModel::setSourceModel(model);

  connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
          SLOT(sourceDataChanged()));
  connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
          SLOT(sourceDataChanged()));

  connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
          SLOT(translatorsAdded(QModelIndex,int,int)));
  connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
          SLOT(translatorsRemoved(QModelIndex,int,int)));
}
int TranslatorsProxyModel::columnCount(const QModelIndex &) const
{
  return 4;
}
QVariant TranslatorsProxyModel::data(const QModelIndex &index, int role) const
{
  // early exit
  if (role != Qt::DisplayRole) {
    return QIdentityProxyModel::data(index, role);
  }
  const QModelIndex ind = this->index(index.row(), 0);
  TranslatorWrapper *trans = translator(ind);
  Q_ASSERT(trans);
  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return Util::addressToString(trans->translator());
    } else if (index.column() == 1) {
      return QString(trans->translator()->metaObject()->className());
    } else if (index.column() == 2) {
      return trans->translator()->objectName();
    } else if (index.column() == 3) {
      return trans->model()->rowCount(QModelIndex());
    }
  }
  return QIdentityProxyModel::data(index, role);
}
QVariant TranslatorsProxyModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
  if (orientation == Qt::Horizontal && section >= 2) {
    if (role == Qt::DisplayRole) {
      if (section == 2) {
        return tr("Name");
      } else if (section == 3) {
        return tr("Translations");
      }
    } else {
      return QVariant();
    }
  }
  return QIdentityProxyModel::headerData(section, orientation, role);
}
QModelIndex TranslatorsProxyModel::mapToSource(
        const QModelIndex &proxyIndex) const
{
  if (proxyIndex.column() >= 2) {
    return QModelIndex();
  } else {
    return QIdentityProxyModel::mapToSource(proxyIndex);
  }
}
QModelIndex TranslatorsProxyModel::index(int row, int column,
                                         const QModelIndex &parent) const
{
  if (column >= 2) {
    if (hasIndex(row, column, parent)) {
      return createIndex(row, column);
    } else {
      return QModelIndex();
    }
  } else {
    return QIdentityProxyModel::index(row, column, parent);
  }
}
Qt::ItemFlags TranslatorsProxyModel::flags(const QModelIndex &index) const
{
  if (index.column() >= 2) {
    return QIdentityProxyModel::flags(
        this->index(index.row(), 0, index.parent()));
  } else {
    return QIdentityProxyModel::flags(index);
  }
}
TranslatorWrapper *TranslatorsProxyModel::translator(const QModelIndex &index)
    const
{
  return qobject_cast<TranslatorWrapper *>(
      index.data(ObjectModel::ObjectRole).value<QObject *>());
}
void TranslatorsProxyModel::sourceDataChanged()
{
  const QModelIndex
      tl = index(0, 2, QModelIndex()),
      br = index(rowCount()-1, 3, QModelIndex());
  if (!tl.isValid() || !br.isValid()) {
    return;
  }
  // needed to make sure these things also update
  emit dataChanged(tl, br,
                   QVector<int>() << Qt::DisplayRole << Qt::EditRole);
}
void TranslatorsProxyModel::translatorsAdded(const QModelIndex &, const int start, const int end)
{
  for (int i = start; i < (end + 1); ++i) {
    connect(translator(index(i, 0))->model(), SIGNAL(rowCountChanged()),
            SLOT(sourceDataChanged()));
    connect(translator(index(i, 0))->model(), SIGNAL(objectNameChanged(QString)),
            SLOT(sourceDataChanged()));
  }
}
void TranslatorsProxyModel::translatorsRemoved(const QModelIndex &, const int start, const int end)
{
  for (int i = start; i < (end + 1); ++i) {
    disconnect(translator(index(i, 0))->model(), SIGNAL(rowCountChanged()),
               this, SLOT(sourceDataChanged()));
    disconnect(translator(index(i, 0))->model(), SIGNAL(objectNameChanged(QString)),
               this, SLOT(sourceDataChanged()));
  }
}
