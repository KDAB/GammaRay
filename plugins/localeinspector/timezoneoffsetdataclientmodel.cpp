/*
  timezoneoffsetdataclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "timezoneoffsetdataclientmodel.h"

using namespace GammaRay;

TimezoneOffsetDataClientModel::TimezoneOffsetDataClientModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

TimezoneOffsetDataClientModel::~TimezoneOffsetDataClientModel() = default;

QVariant TimezoneOffsetDataClientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Transition Time (UTC)");
        case 1:
            return tr("Offset to UTC");
        case 2:
            return tr("Standard Time Offset");
        case 3:
            return tr("DST Offset");
        case 4:
            return tr("Abbreviation");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
