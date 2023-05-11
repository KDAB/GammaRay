/*
  recursiveproxymodelbase.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_RECURSIVEPROXYMODELBASE_H
#define GAMMARAY_RECURSIVEPROXYMODELBASE_H

#include "gammaray_common_export.h"

#include <QtGlobal>

/**
 * NOTE: This class can be removed once we raise our minimum Qt version to 5.10 or above
 */

#if QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
#include <kde/krecursivefilterproxymodel.h>
#define GAMMARAY_PROXY_BASE_CLASS KRecursiveFilterProxyModel
#else
#include <QSortFilterProxyModel>
#define GAMMARAY_PROXY_BASE_CLASS QSortFilterProxyModel
#endif

class GAMMARAY_COMMON_EXPORT RecursiveProxyModelBase : public
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
                                                       KRecursiveFilterProxyModel
#else
                                                       QSortFilterProxyModel
#endif
{
public:
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    using KRecursiveFilterProxyModel::KRecursiveFilterProxyModel;
#else
    RecursiveProxyModelBase(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
        setRecursiveFilteringEnabled(true);
    }
#endif

    virtual bool acceptRow(int sourceRow, const QModelIndex &sourceParent) const;
};
#endif
