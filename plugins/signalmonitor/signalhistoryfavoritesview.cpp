/*
  signalhistoryfavoritesview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

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
#include "signalhistoryfavoritesview.h"
#include "signalhistorymodel.h"

#include <common/objectbroker.h>
#include <common/favoriteobjectinterface.h>
#include <common/objectmodel.h>

#include <QSortFilterProxyModel>
#include <QMenu>

using namespace GammaRay;

class SignalHistoryFavoritesModel : public QSortFilterProxyModel
{
public:
    explicit SignalHistoryFavoritesModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        setFilterRole(ObjectModel::IsFavoriteRole);
        setFilterKeyColumn(0);

        auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SignalHistoryModel"));
        setSourceModel(sourceModel);
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        auto idx = sourceModel()->index(source_row, SignalHistoryModel::ObjectColumn, source_parent);
        return idx.data(ObjectModel::IsFavoriteRole).toBool();
    }
};

SignalHistoryFavoritesView::SignalHistoryFavoritesView(QWidget *parent)
    : GammaRay::SignalHistoryView(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setRootIsDecorated(false);
    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setModel(new SignalHistoryFavoritesModel(this));

    connect(this, &SignalHistoryFavoritesView::customContextMenuRequested,
            this, &SignalHistoryFavoritesView::onCustomContextMenuRequested);

    if (model() && model()->rowCount() == 0)
        setHidden(true);
}

void SignalHistoryFavoritesView::setModel(QAbstractItemModel *m)
{
    if (model())
        disconnect(model(), &QAbstractItemModel::rowsRemoved, this,
                   &SignalHistoryFavoritesView::rowsRemoved);

    SignalHistoryView::setModel(m);

    connect(m, &QAbstractItemModel::rowsRemoved, this,
            &SignalHistoryFavoritesView::rowsRemoved);
}


void SignalHistoryFavoritesView::rowsRemoved(const QModelIndex &, int, int)
{
    if (model() && model()->rowCount() == 0)
        setHidden(true);
}

void SignalHistoryFavoritesView::rowsInserted(const QModelIndex &idx, int s, int e)
{
    if (isHidden())
        setHidden(false);
    SignalHistoryView::rowsInserted(idx, s, e);
}

void SignalHistoryFavoritesView::onCustomContextMenuRequested(QPoint pos)
{
    auto index = indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);
    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    menu.addAction(tr("Remove from favorites"), this, [objectId]{
        ObjectBroker::object<FavoriteObjectInterface*>()->unfavoriteObject(objectId);
    });

    menu.exec(viewport()->mapToGlobal(pos));
}
