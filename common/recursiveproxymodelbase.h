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

#include <QSortFilterProxyModel>

class GAMMARAY_COMMON_EXPORT RecursiveProxyModelBase : public QSortFilterProxyModel
{
public:
    RecursiveProxyModelBase(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
        setRecursiveFilteringEnabled(true);
    }

    virtual bool acceptRow(int sourceRow, const QModelIndex &sourceParent) const;

    // compat: always override acceptRow in subclasses
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const final;
};
#endif
