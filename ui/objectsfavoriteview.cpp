/*
  objectsfavoriteview.h

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
#include "objectsfavoriteview.h"

using namespace GammaRay;

ObjectsFavoriteView::ObjectsFavoriteView(QWidget *parent)
    : FavoritesItemView<DeferredTreeView>(parent)
{
    connect(this, &QTreeView::clicked, this, &ObjectsFavoriteView::onIndexClicked);
}

void ObjectsFavoriteView::setSourceModel(QAbstractItemModel *model)
{
    auto proxyModel = new FavoritesModel(model, this);
    proxyModel->setRecursiveFilteringEnabled(true);
    setModel(proxyModel);
}

void ObjectsFavoriteView::setObjectsView(GammaRay::DeferredTreeView *view)
{
    m_objectsView = view;
    // both views must have same sourceModel
    auto sm = view->model();
    auto m = model();
#define proxy(m) qobject_cast<QAbstractProxyModel*>(m)
    Q_ASSERT(proxy(sm)->sourceModel() == proxy(m)->sourceModel());
#undef proxy
}

void ObjectsFavoriteView::onIndexClicked(const QModelIndex &idx)
{
    if (!idx.isValid() || !m_objectsView)
        return;

    auto favProxyModel = qobject_cast<QAbstractProxyModel*>(model());
    auto sourceIdx = favProxyModel->mapToSource(idx);

    auto objProxyModel = qobject_cast<QAbstractProxyModel*>(m_objectsView->model());
    auto idxToSel = objProxyModel->mapFromSource(sourceIdx);

    m_objectsView->selectionModel()->select(idxToSel, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
