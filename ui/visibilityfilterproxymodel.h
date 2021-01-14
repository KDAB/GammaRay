/*
  objectidfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Manfred Tonch <manfred.tonch@kdab.com>

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
