/*
  probeabimodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    return m_abis.size();
}

int ProbeABIModel::indexOfBestMatchingABI(const ProbeABI &targetABI) const
{
    if (!targetABI.isValid())
        return -1;

    const ProbeABI bestMatchingABI = ProbeFinder::findBestMatchingABI(targetABI, m_abis);
    return m_abis.indexOf(bestMatchingABI);
}
