/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

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

#ifndef KRECURSIVEFILTERPROXYMODEL_H
#define KRECURSIVEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "kitemmodels_export.h"

class KRecursiveFilterProxyModelPrivate;

/**
  @class KRecursiveFilterProxyModel krecursivefilterproxymodel.h KRecursiveFilterProxyModel

  @brief Implements recursive filtering of models

  Until Qt 5.10, QSortFilterProxyModel did not recurse when invoking a filtering stage, so that
  if a particular row is filtered out, its children are not even checked to see if they match the filter.

  If you can depend on Qt >= 5.10, then just use QSortFilterProxyModel::setRecursiveFilteringEnabled(true),
  and you don't need to use KRecursiveFilterProxyModel.

  For example, given a source model:

  @verbatim
    - A
    - B
    - - C
    - - - D
    - - - - E
    - - - F
    - - G
    - - H
    - I
  @endverbatim

  If a QSortFilterProxyModel is used with a filter matching A, D, G and I, the QSortFilterProxyModel will contain

  @verbatim
    - A
    - I
  @endverbatim

  That is, even though D and E match the filter, they are not represented in the proxy model because B does not
  match the filter and is filtered out.

  The KRecursiveFilterProxyModel checks child indexes for filter matching and ensures that all matching indexes
  are represented in the model.

  In the above example, the KRecursiveFilterProxyModel will contain

  @verbatim
    - A
    - B
    - - C
    - - - D
    - - G
    - I
  @endverbatim

  That is, the leaves in the model match the filter, but not necessarily the inner branches.

  QSortFilterProxyModel provides the virtual method filterAcceptsRow to allow custom filter implementations.
  Custom filter implementations can be written for KRecuriveFilterProxyModel using the acceptRow virtual method.

  Note that using this proxy model is additional overhead compared to QSortFilterProxyModel as every index in the
  model must be visited and queried.

  @author Stephen Kelly <steveire@gmail.com>

  @since 4.5

*/
class KITEMMODELS_EXPORT KRecursiveFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /**
      Constructor
    */
    explicit KRecursiveFilterProxyModel(QObject *parent = nullptr);

    /**
      Destructor
    */
    ~KRecursiveFilterProxyModel() override;

    /** @reimp */
    void setSourceModel(QAbstractItemModel *model) override;

    /**
     * @reimplemented
     */
    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits = 1,
                                  Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const override;

protected:
    /**
      Reimplement this method for custom filtering strategies.
    */
    virtual bool acceptRow(int sourceRow, const QModelIndex &sourceParent) const;

    /** @reimp */
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    KRecursiveFilterProxyModelPrivate *const d_ptr;

private:
    //@cond PRIVATE
    Q_DECLARE_PRIVATE(KRecursiveFilterProxyModel)

    Q_PRIVATE_SLOT(d_func(), void sourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QVector<int> &roles = QVector<int>()))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsAboutToBeInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsRemoved(const QModelIndex &source_parent, int start, int end))
    //@endcond
};

#endif

