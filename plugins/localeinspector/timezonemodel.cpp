/*
  timezonemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "timezonemodel.h"
#include "timezonemodelroles.h"

#include <QLocale>
#include <QTimeZone>

using namespace GammaRay;

TimezoneModel::TimezoneModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

TimezoneModel::~TimezoneModel() = default;

int TimezoneModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TimezoneModelColumns::COUNT;
}

int TimezoneModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (m_ids.isEmpty())
        m_ids = QTimeZone::availableTimeZoneIds();
    return m_ids.count();
}

static QString displayNameForAllTimeTypes(const QTimeZone &tz, QTimeZone::NameType nameType)
{
    if (!tz.hasDaylightTime()) {
        return tz.displayName(QTimeZone::StandardTime, nameType);
    }
    return tz.displayName(QTimeZone::StandardTime, nameType) + QLatin1String(" / ")
        + tz.displayName(QTimeZone::DaylightTime, nameType) + QLatin1String(" / ")
        + tz.displayName(QTimeZone::GenericTime, nameType);
}

QVariant TimezoneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const QTimeZone tz(m_ids.at(index.row()));
        switch (index.column()) {
        case TimezoneModelColumns::IanaIdColumn:
            return tz.id();
        case TimezoneModelColumns::CountryColumn:
            return QLocale::countryToString(tz.country());
        case TimezoneModelColumns::StandardDisplayNameColumn:
            return tz.displayName(QTimeZone::StandardTime);
        case TimezoneModelColumns::DSTColumn:
            return tz.hasDaylightTime();
        case TimezoneModelColumns::WindowsIdColumn:
            return QTimeZone::ianaIdToWindowsId(tz.id());
        }
    } else if (role == Qt::ToolTipRole) {
        const QTimeZone tz(m_ids.at(index.row()));
        switch (index.column()) {
        case 0:
            return tz.comment();
        case TimezoneModelColumns::StandardDisplayNameColumn:
            return QString(displayNameForAllTimeTypes(tz, QTimeZone::LongName) + QLatin1Char('\n')
                           + displayNameForAllTimeTypes(tz, QTimeZone::ShortName) + QLatin1Char('\n')
                           + displayNameForAllTimeTypes(tz, QTimeZone::OffsetName));
        default:
            return {};
        }
    } else if (role == TimezoneModelRoles::LocalZoneRole && index.column() == 0) {
        if (m_ids.at(index.row()) == QTimeZone::systemTimeZoneId())
            return true;
        return QVariant();
    }

    return QVariant();
}
