/*
  favoritesitemview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

    auto pm = static_cast<KModelIndexProxyMapper *>(m_proxyMapper);
    auto sourceIdx = pm->mapRightToLeft(idx);

    m_sourceView->selectionModel()->select(sourceIdx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
