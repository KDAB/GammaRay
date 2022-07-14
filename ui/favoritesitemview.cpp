/*
  favoritesitemview.cpp

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
#include "favoritesitemview.h"
#include "kde/kdescendantsproxymodel.h"
#include "kde/kmodelindexproxymapper.h"

using namespace GammaRay;

void ObjectsFavoriteView::setModel(QAbstractItemModel *model)
{
    // We have three proxy models sitting in the line
    // FavoritesModel -> KDescendantsProxyModel -> FavoritesModel -> View
    // 1. Favorites model filters out all the non-favorites
    // 2. This small list of favorites is then flattened by kdescendantsproxymodel
    // 3. The flattened list is then refiltered to remove any parents
    //
    // We can do it with 2 models as well i.e.,
    // KDescendantsProxyModel -> FavoritesModel -> View
    // However, this makes things super slow

    auto proxyModel1 = static_cast<FavoritesModel*>(model);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    proxyModel1->setRecursiveFilteringEnabled(true);
#endif

    auto flatteningModel = new KDescendantsProxyModel(this);
    flatteningModel->setSourceModel(proxyModel1);

    auto proxyModel2 = new FavoritesModel(flatteningModel, this);

    m_proxyMapper = new KModelIndexProxyMapper(proxyModel1->sourceModel(), proxyModel2, this);

    FavoritesItemView<DeferredTreeView>::setModel(proxyModel2);
}

void ObjectsFavoriteView::onIndexClicked(const QModelIndex &idx)
{
    if (!idx.isValid() || !m_sourceView || !m_proxyMapper)
        return;

    auto pm = static_cast<KModelIndexProxyMapper*>(m_proxyMapper);
    auto sourceIdx = pm->mapRightToLeft(idx);

    m_sourceView->selectionModel()->select(sourceIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
