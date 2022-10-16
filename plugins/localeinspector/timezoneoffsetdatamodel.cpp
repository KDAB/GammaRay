/*
  timezoneoffsetdatamodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "timezoneoffsetdatamodel.h"

#include <QDebug>

using namespace GammaRay;

TimezoneOffsetDataModel::TimezoneOffsetDataModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

TimezoneOffsetDataModel::~TimezoneOffsetDataModel() = default;

void TimezoneOffsetDataModel::setTimezone(const QTimeZone &tz)
{
    if (!m_offsets.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_offsets.size() - 1);
        m_offsets.clear();
        endRemoveRows();
    }

    QVector<QTimeZone::OffsetData> offsets;
    offsets.reserve(60);

    const auto start = QDateTime::currentDateTime();

    auto offset = tz.offsetData(start);
    for (int i = 0; i < 30; ++i) {
        offset = tz.previousTransition(offset.atUtc);
        if (!offset.atUtc.isValid())
            break;
        offsets.prepend(offset);
    }

    offset = tz.offsetData(start);
    for (int i = 0; i < 30; ++i) {
        offset = tz.nextTransition(offset.atUtc);
        if (!offset.atUtc.isValid())
            break;
        offsets.push_back(offset);
    }

    if (!offsets.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, offsets.size() - 1);
        m_offsets = offsets;
        endInsertRows();
    }
}

int TimezoneOffsetDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int TimezoneOffsetDataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_offsets.size();
}

QVariant TimezoneOffsetDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto offset = m_offsets.at(index.row());
        switch (index.column()) {
        case 0:
            return offset.atUtc;
        case 1:
            return offset.offsetFromUtc;
        case 2:
            return offset.standardTimeOffset;
        case 3:
            return offset.daylightTimeOffset;
        case 4:
            return offset.abbreviation;
        }
    }

    return QVariant();
}
