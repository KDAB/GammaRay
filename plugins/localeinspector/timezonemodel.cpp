/*
  timezonemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "timezonemodel.h"

#include <QLocale>
#include <QTimeZone>

using namespace GammaRay;

TimezoneModel::TimezoneModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

TimezoneModel::~TimezoneModel()
{
}

int TimezoneModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int TimezoneModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    if (m_ids.isEmpty())
        m_ids = QTimeZone::availableTimeZoneIds();
    return m_ids.count();
}

QVariant TimezoneModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const QTimeZone tz(m_ids.at(index.row()));
        switch (index.column()) {
            case 0:
                return tz.id();
            case 1:
                return QLocale::countryToString(tz.country());
            case 2:
                return tz.displayName(QTimeZone::StandardTime);
            case 3:
                return tz.hasDaylightTime();
            case 4:
                return QTimeZone::ianaIdToWindowsId(tz.id());
        }
    } else if (role == Qt::ToolTipRole && index.column() == 0) {
        const QTimeZone tz(m_ids.at(index.row()));
        return tz.comment();
    }

    return QVariant();
}
