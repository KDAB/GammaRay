/*
  favoritesitemview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    auto proxyModel1 = static_cast<FavoritesModel *>(model);
    proxyModel1->setRecursiveFilteringEnabled(true);

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

    auto pm = static_cast<KModelIndexProxyMapper *>(m_proxyMapper);
    auto sourceIdx = pm->mapRightToLeft(idx);

    m_sourceView->selectionModel()->select(sourceIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
