/*
  recursiveproxymodelbase.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#include "recursiveproxymodelbase.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
bool RecursiveProxyModelBase::acceptRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // delegate to base class
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
#endif

bool RecursiveProxyModelBase::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // delegate to acceptRow
    return acceptRow(sourceRow, sourceParent);
}
