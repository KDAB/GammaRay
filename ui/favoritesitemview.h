/*
  favoritesitemview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_FAVORITESITEMVIEW_H
#define GAMMARAY_FAVORITESITEMVIEW_H

#include <common/objectbroker.h>
#include <common/favoriteobjectinterface.h>
#include <common/objectmodel.h>
#include <ui/deferredtreeview.h>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMenu>
#include <QPointer>
#include <QSortFilterProxyModel>

namespace GammaRay {

class FavoritesModel final : public QSortFilterProxyModel
{
public:
    explicit FavoritesModel(QAbstractItemModel *sourceModel, QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        setFilterRole(ObjectModel::IsFavoriteRole);
        setFilterKeyColumn(0);
        setSourceModel(sourceModel);
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        auto idx = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
        return idx.data(ObjectModel::IsFavoriteRole).toBool();
    }
};

template<typename Base>
class FavoritesItemView : public Base
{
public:
    FavoritesItemView(QWidget *parent = nullptr)
        : Base(parent)
    {
        Base::setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        Base::setContextMenuPolicy(Qt::CustomContextMenu);

        Base::connect(this, &Base::customContextMenuRequested,
                      this, &FavoritesItemView::onCustomContextMenuRequested);
        Base::connect(this, &Base::clicked, this, &FavoritesItemView<Base>::onIndexClicked);
    }

    void setSourceView(QAbstractItemView *view)
    {
        setModel(new FavoritesModel(view->model(), this));
        m_sourceView = view;
    }

protected:
    void setModel(QAbstractItemModel *m) override
    {
        if (Base::model())
            Base::disconnect(Base::model(), &QAbstractItemModel::rowsRemoved, this,
                             &FavoritesItemView::rowsRemoved);

        Base::setModel(m);
        if (Base::model() && Base::model()->rowCount() == 0)
            Base::setHidden(true);

        Base::connect(m, &QAbstractItemModel::rowsRemoved, this, &FavoritesItemView::onRowsRemoved);
    }

private:
    void onRowsRemoved()
    {
        if (Base::model() && Base::model()->rowCount() == 0)
            Base::setHidden(true);
    }

    void rowsInserted(const QModelIndex &parent, int s, int e) override
    {
        if (Base::isHidden())
            Base::setHidden(false);
        Base::rowsInserted(parent, s, e);
    }

    void onCustomContextMenuRequested(const QPoint &pos)
    {
        auto index = Base::indexAt(pos);
        if (!index.isValid() || !index.data(ObjectModel::IsFavoriteRole).toBool())
            return;
        index = index.sibling(index.row(), 0);
        const auto objectId = index.data(ObjectModel::ObjectIdRole).template value<ObjectId>();
        if (objectId.isNull())
            return;

        QMenu menu;
        menu.addAction(Base::tr("Remove from favorites"), this, [objectId] {
            ObjectBroker::object<FavoriteObjectInterface *>()->unfavoriteObject(objectId);
        });

        menu.exec(Base::viewport()->mapToGlobal(pos));
    }

    virtual void onIndexClicked(const QModelIndex &idx)
    {
        if (!idx.isValid() || !m_sourceView)
            return;

        auto favProxyModel = qobject_cast<QAbstractProxyModel *>(Base::model());
        auto sourceIdx = favProxyModel->mapToSource(idx);
        m_sourceView->selectionModel()->select(sourceIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }

protected:
    QPointer<QAbstractItemView> m_sourceView;
};

class GAMMARAY_UI_EXPORT ObjectsFavoriteView final : public FavoritesItemView<DeferredTreeView>
{
    Q_OBJECT
public:
    using FavoritesItemView<DeferredTreeView>::FavoritesItemView;

private:
    void setModel(QAbstractItemModel *model) override;
    void onIndexClicked(const QModelIndex &idx) override;

private:
    QObject *m_proxyMapper = nullptr;
};

}

#endif
