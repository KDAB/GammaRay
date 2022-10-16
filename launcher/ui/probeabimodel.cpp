/*
  probeabimodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
