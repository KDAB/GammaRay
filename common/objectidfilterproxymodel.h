/*
  objectidfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTIDFILTERPROXYMODEL_H
#define GAMMARAY_OBJECTIDFILTERPROXYMODEL_H

#include "gammaray_common_export.h"
#include "3rdparty/kde/krecursivefilterproxymodel.h"

#include <common/objectid.h>

namespace GammaRay {

/**
 * @brief A KRecursiveFilterProxyModel for ObjectIds.
 *
 * Filter in and sort according to the objects list.
 */
class GAMMARAY_COMMON_EXPORT ObjectIdsFilterProxyModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectIdsFilterProxyModel(QObject *parent = nullptr);

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    GammaRay::ObjectIds ids() const;
    void setIds(const GammaRay::ObjectIds &ids);

protected:
    /**
     * Determines if the item in the specified row can be included in the model.
     * @param source_row is a non-zero integer representing the row of the item.
     * @param source_parent is the parent QModelIndex for this model.
     * @return true if the item in the row can be included in the model;
     *         otherwise returns false.
     */
    bool acceptRow(int source_row, const QModelIndex &source_parent) const override;

    /**
     * Determines if the specified ObjectID can be included in the model.
     * @param id is a ref to the ObjectId to test.
     * @return true if the ObjectId can be included in the model; false otherwise.
     */
    bool filterAcceptsObjectId(const GammaRay::ObjectId &id) const;

private:
    GammaRay::ObjectIds m_ids;
};

}

#endif // GAMMARAY_OBJECIDFILTERPROXYMODEL_H
