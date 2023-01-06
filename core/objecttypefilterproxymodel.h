/*
  objecttypefilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
/**
  @file
  This file is part of the GammaRay Plugin API and declares a template
  for an ObjectTypeFilterProxyModel class.

  @brief
  Declares a template for an ObjectTypeFilterProxyModel class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H
#define GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H

#include "gammaray_core_export.h"
#include "objectmodelbase.h"

#include <QSortFilterProxyModel>

namespace GammaRay {
/**
 * @brief A QSortFilterProxyModel for generic Objects.
 */
class GAMMARAY_CORE_EXPORT ObjectFilterProxyModelBase : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectFilterProxyModelBase(QObject *parent = nullptr);

    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

protected:
    /**
     * Determines if the item in the specified row can be included in the model.
     * @param source_row is a non-zero integer representing the row of the item.
     * @param source_parent is the parent QModelIndex for this model.
     * @return true if the item in the row can be included in the model;
     *         otherwise returns false.
     */
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    /**
     * Determines if the specified QObject can be included in the model.
     * @param object is a pointer to the QObject to test.
     * @return true if the QObject can be included in the model; false otherwise.
     */
    virtual bool filterAcceptsObject(QObject *object) const = 0;
};

/**
 * @brief A templated generic ObjectFilterProxyModelBase for some data type.
 */
template<typename T1, typename T2 = T1>
class ObjectTypeFilterProxyModel : public ObjectFilterProxyModelBase
{
public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectTypeFilterProxyModel(QObject *parent = nullptr)
        : ObjectFilterProxyModelBase(parent)
    {
    }

protected:
    bool filterAcceptsObject(QObject *object) const override
    {
        return qobject_cast<T1 *>(object) || qobject_cast<T2 *>(object);
    }
};

}

#endif // GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H
