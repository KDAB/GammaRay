/*
    SPDX-FileCopyrightText: 2009 Stephen Kelly <steveire@gmail.com>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDESCENDANTSPROXYMODEL_P_H
#define KDESCENDANTSPROXYMODEL_P_H

#include <QAbstractProxyModel>

#include "kitemmodels_export.h"

#include <memory>

class KDescendantsProxyModelPrivate;

/**
@class KDescendantsProxyModel kdescendantsproxymodel.h KDescendantsProxyModel
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
descProxy->setAncestorSeparator(QString(" / "));
view->setModel(descProxy);
@endcode
\image html descendantentitiesproxymodel-withansecnames.png "A KDescendantsProxyModel with ancestor names."
@since 4.6
@author Stephen Kelly <steveire@gmail.com>
*/
class KITEMMODELS_EXPORT KDescendantsProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

    /**
     * @since 5.62
     */
    Q_PROPERTY(QAbstractItemModel *model READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    /**
     * @since 5.62
     */
    Q_PROPERTY(bool displayAncestorData READ displayAncestorData WRITE setDisplayAncestorData NOTIFY displayAncestorDataChanged)
    /**
     * @since 5.62
     */
    Q_PROPERTY(QString ancestorSeparator READ ancestorSeparator WRITE setAncestorSeparator NOTIFY ancestorSeparatorChanged)

    /**
     * If true, all the nodes in the whole tree will be expanded upon loading and all items
     * of the source model will be shown in the proxy.
     * The default value is true.
     * @since 5.74
     */
    Q_PROPERTY(bool expandsByDefault READ expandsByDefault WRITE setExpandsByDefault NOTIFY expandsByDefaultChanged)

public:
    enum AdditionalRoles {
        // Note: use printf "0x%08X\n" $(($RANDOM*$RANDOM))
        // to define additional roles.
        LevelRole = 0x14823F9A,
        ExpandableRole = 0x1CA894AD,
        ExpandedRole = 0x1E413DA4,
        HasSiblingsRole = 0x1633CE0C,
    };

    /**
     * Creates a new descendant entities proxy model.
     *
     * @param parent The parent object.
     */
    explicit KDescendantsProxyModel(QObject *parent = nullptr);

    /**
     * Destroys the descendant entities proxy model.
     */
    ~KDescendantsProxyModel() override;

    /**
     * Sets the source @p model of the proxy.
     */
    void setSourceModel(QAbstractItemModel *model) override;

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

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int, int, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * If true, all the nodes in the whole tree will be expanded upon loading (default)
     * @param expand whether we want everything expanded upon load
     * @since 5.74
     */
    void setExpandsByDefault(bool expand);

    /**
     * @returns true if all the tree nodes are expanded by default upon loading
     * @since 5.74
     */
    bool expandsByDefault() const;

    /**
     * @returns true if the source index is mapped in the proxy as expanded, therefore it will show its children
     * @since 5.74
     */
    bool isSourceIndexExpanded(const QModelIndex &sourceIndex) const;

    /**
     * @returns true if the source index is visible in the proxy, meaning all its parent hierarchy is expanded.
     * @since 5.74
     */
    bool isSourceIndexVisible(const QModelIndex &sourceIndex) const;

    /**
     * Maps a source index as expanded in the proxy, all its children will become visible.
     * @param sourceIndex an idex of the source model.
     * @since 5.74
     */
    void expandSourceIndex(const QModelIndex &sourceIndex);

    /**
     * Maps a source index as collapsed in the proxy, all its children will be hidden.
     * @param sourceIndex an idex of the source model.
     * @since 5.74
     */
    void collapseSourceIndex(const QModelIndex &sourceIndex);

    Qt::DropActions supportedDropActions() const override;

    /**
    Reimplemented to match all descendants.
    */
    virtual QModelIndexList match(const QModelIndex &start,
                                  int role,
                                  const QVariant &value,
                                  int hits = 1,
                                  Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const override;

Q_SIGNALS:
    void sourceModelChanged();
    void displayAncestorDataChanged();
    void ancestorSeparatorChanged();
    void expandsByDefaultChanged(bool expands);
    void sourceIndexExpanded(const QModelIndex &sourceIndex);
    void sourceIndexCollapsed(const QModelIndex &sourceIndex);

private:
    Q_DECLARE_PRIVATE(KDescendantsProxyModel)
    //@cond PRIVATE
    KDescendantsProxyModelPrivate *const d_ptr;

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
