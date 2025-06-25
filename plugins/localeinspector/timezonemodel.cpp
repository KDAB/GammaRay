/*
  timezonemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    return m_ids.size();
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
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
            return QLocale::countryToString(tz.country());
#else
            return QLocale::territoryToString(tz.territory());
#endif
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
