/*
  visibilityfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Manfred Tonch <manfred.tonch@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_VISIBILITYFILTERPROXYMODEL_H
#define GAMMARAY_VISIBILITYFILTERPROXYMODEL_H

#include "gammaray_common_export.h"
#include "3rdparty/kde/krecursivefilterproxymodel.h"

namespace GammaRay {

/**
 * @brief A KRecursiveFilterProxyModel for ObjectIds.
 *
 * Filter in and sort according to the objects list.
 */
class VisibilityFilterProxyModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit VisibilityFilterProxyModel(QObject *parent = nullptr);

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void setFlagRole(int flagRole);
    void setInvisibleMask(int invisibleMask);

public slots:
    void setHideItems(bool hideItems);

protected:
    /**
     * Determines if the item in the specified row can be included in the model.
     * @param source_row is a non-zero integer representing the row of the item.
     * @param source_parent is the parent QModelIndex for this model.
     * @return true if the item in the row can be included in the model;
     *         otherwise returns false.
     */
    bool acceptRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool m_hideItems;
    int m_flagRole;
    int m_invisibleMask;
};

}

#endif // GAMMARAY_VISIBILITYFILTERPROXYMODEL_H
