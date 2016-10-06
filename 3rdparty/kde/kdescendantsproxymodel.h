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

#ifndef KDESCENDANTSPROXYMODEL_P_H
#define KDESCENDANTSPROXYMODEL_P_H

#include <QtCore/QAbstractProxyModel>

class KDescendantsProxyModelPrivate;

#include "kitemmodels_export.h"

/**
@brief Proxy Model for restructuring a Tree into a list.

A KDescendantsProxyModel may be used to alter how the items in the tree are presented.

Given a model which is represented as a tree:

\image html entitytreemodel.png "A plain EntityTreeModel in a view"

The KDescendantsProxyModel restructures the sourceModel to represent it as a flat list.

@code
// ... Create an entityTreeModel
KDescendantsProxyModel *descProxy = new KDescendantsProxyModel(this);
descProxy->setSourceModel(entityTree);
view->setModel(descProxy);
@endcode

\image html descendantentitiesproxymodel.png "A KDescendantsProxyModel."

KDescendantEntitiesProxyModel can also display the ancestors of the index in the source model as part of its display.

@code
// ... Create an entityTreeModel
KDescendantsProxyModel *descProxy = new KDescendantsProxyModel(this);
descProxy->setSourceModel(entityTree);

// #### This is new
descProxy->setDisplayAncestorData(true);
descProxy->setDisplayAncestorSeparator(QString(" / "));

view->setModel(descProxy);

@endcode

\image html descendantentitiesproxymodel-withansecnames.png "A KDescendantsProxyModel with ancestor names."

@since 4.6
@author Stephen Kelly <steveire@gmail.com>
*/
class KITEMMODELS_EXPORT KDescendantsProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:

    /**
     * Creates a new descendant entities proxy model.
     *
     * @param parent The parent object.
     */
    explicit KDescendantsProxyModel(QObject *parent = 0);

    /**
     * Destroys the descendant entities proxy model.
     */
    virtual ~KDescendantsProxyModel();

    /**
     * Sets the source @p model of the proxy.
     */
    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    /**
     * @deprecated
     *
     * This method does nothing.
     */
    void setRootIndex(const QModelIndex &index);

    /**
     * Set whether to show ancestor data in the model. If @p display is true, then
     * a source model which is displayed as
     *
     * @code
     *  -> "Item 0-0" (this is row-depth)
     *  -> -> "Item 0-1"
     *  -> -> "Item 1-1"
     *  -> -> -> "Item 0-2"
     *  -> -> -> "Item 1-2"
     *  -> "Item 1-0"
     * @endcode
     *
     * will be displayed as
     *
     * @code
     *  -> *Item 0-0"
     *  -> "Item 0-0 / Item 0-1"
     *  -> "Item 0-0 / Item 1-1"
     *  -> "Item 0-0 / Item 1-1 / Item 0-2"
     *  -> "Item 0-0 / Item 1-1 / Item 1-2"
     *  -> "Item 1-0"
     * @endcode
     *
     * If @p display is false, the proxy will show
     *
     * @code
     *  -> *Item 0-0"
     *  -> "Item 0-1"
     *  -> "Item 1-1"
     *  -> "Item 0-2"
     *  -> "Item 1-2"
     *  -> "Item 1-0"
     * @endcode
     *
     * Default is false.
     */
    void setDisplayAncestorData(bool display);

    /**
     * Whether ancestor data will be displayed.
     */
    bool displayAncestorData() const;

    /**
     * Sets the ancestor @p separator used between data of ancestors.
     */
    void setAncestorSeparator(const QString &separator);

    /**
     * Separator used between data of ancestors.
     */
    QString ancestorSeparator() const;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;

    /**
    Reimplemented to match all descendants.
    */
    virtual QModelIndexList match(const QModelIndex &start, int role, const QVariant &value,
                                  int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(KDescendantsProxyModel)
    //@cond PRIVATE
    KDescendantsProxyModelPrivate *d_ptr;

    Q_PRIVATE_SLOT(d_func(), void sourceRowsAboutToBeInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsAboutToBeRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int))
    Q_PRIVATE_SLOT(d_func(), void sourceRowsMoved(const QModelIndex &, int, int, const QModelIndex &, int))
    Q_PRIVATE_SLOT(d_func(), void sourceModelAboutToBeReset())
    Q_PRIVATE_SLOT(d_func(), void sourceModelReset())
    Q_PRIVATE_SLOT(d_func(), void sourceLayoutAboutToBeChanged())
    Q_PRIVATE_SLOT(d_func(), void sourceLayoutChanged())
    Q_PRIVATE_SLOT(d_func(), void sourceDataChanged(const QModelIndex &, const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void sourceModelDestroyed())

    Q_PRIVATE_SLOT(d_func(), void processPendingParents())

    // Make these private, they shouldn't be called by applications
//   virtual bool insertRows(int , int, const QModelIndex & = QModelIndex());
//   virtual bool insertColumns(int, int, const QModelIndex & = QModelIndex());
//   virtual bool removeRows(int, int, const QModelIndex & = QModelIndex());
//   virtual bool removeColumns(int, int, const QModelIndex & = QModelIndex());

    //@endcond
};

#endif
