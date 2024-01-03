/*
  visibilityfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Manfred Tonch <manfred.tonch@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_VISIBILITYFILTERPROXYMODEL_H
#define GAMMARAY_VISIBILITYFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {

/**
 * @brief A RecursiveFilterProxyModel for ObjectIds.
 *
 * Filter in and sort according to the objects list.
 */
class VisibilityFilterProxyModel : public QSortFilterProxyModel
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
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool m_hideItems;
    int m_flagRole;
    int m_invisibleMask;
};

}

#endif // GAMMARAY_VISIBILITYFILTERPROXYMODEL_H
