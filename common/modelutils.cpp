/*
  modelutils.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "modelutils.h"

#include <QAbstractItemModel>

using namespace GammaRay;

QModelIndexList ModelUtils::match(const QModelIndex &start, int role, MatchAcceptor accept,
                                  int hits, Qt::MatchFlags flags)
{
    if (!start.isValid() || role < 0)
        return QModelIndexList();

    const QAbstractItemModel *model = start.model();
    const QModelIndex parentIndex = model->parent(start);
    bool recurse = flags & Qt::MatchRecursive;
    bool wrap = flags & Qt::MatchWrap;
    bool allHits = (hits == -1);
    int from = start.row();
    int to = model->rowCount(parentIndex);

    QModelIndexList result;

    // iterates twice if wrapping
    for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {
        for (int r = from; (r < to) && (allHits || result.count() < hits); ++r) {
            QModelIndex idx = model->index(r, start.column(), parentIndex);
            if (!idx.isValid())
                continue;

            const QVariant v = model->data(idx, role);
            if (accept(v))
                result << idx;

            // search the hierarchy
            if (recurse && model->hasChildren(idx)) {
                result += match(model->index(0, idx.column(), idx), role,
                                accept, (allHits ? -1 : hits - result.count()), flags);
            }
        }

        // prepare for the next iteration
        from = 0;
        to = start.row();
    }

    return result;
}
