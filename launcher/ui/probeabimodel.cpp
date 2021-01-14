/*
  probeabimodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "probeabimodel.h"

#include <launcher/core/probefinder.h>

using namespace GammaRay;

ProbeABIModel::ProbeABIModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_abis = ProbeFinder::listProbeABIs();
}

ProbeABIModel::~ProbeABIModel() = default;

QVariant ProbeABIModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ProbeABI &abi = m_abis.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return abi.displayString();
    case Qt::UserRole:
        return QVariant::fromValue(abi);
    }

    return QVariant();
}

int ProbeABIModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_abis.count();
}

int ProbeABIModel::indexOfBestMatchingABI(const ProbeABI &targetABI) const
{
    if (!targetABI.isValid())
        return -1;

    const ProbeABI bestMatchingABI = ProbeFinder::findBestMatchingABI(targetABI, m_abis);
    return m_abis.indexOf(bestMatchingABI);
}
