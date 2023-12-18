/*
  sysinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "sysinfomodel.h"

#include <QLibraryInfo>
#include <QSysInfo>

using namespace GammaRay;

struct sysinfo_t
{
    QString (*func)();
    const char *name;
};

#define S(x)            \
    {                   \
        QSysInfo::x, #x \
    }
static const sysinfo_t sysInfoTable[] = {
#if !defined(Q_CC_MSVC) || _MSC_VER > 1600 // krazy:exclude=cpp to deal with older MS compilers
    { []() { return QString::fromLatin1(QLibraryInfo::build()); }, "build" },
#endif
    S(buildAbi),
    S(buildCpuArchitecture),
    S(currentCpuArchitecture),
    S(kernelType),
    S(kernelVersion),
    S(machineHostName),
    S(prettyProductName),
    S(productType),
    S(productVersion)
};
#undef S
static const auto sysInfoTableSize = sizeof(sysInfoTable) / sizeof(sysinfo_t);

SysInfoModel::SysInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int SysInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int SysInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return sysInfoTableSize;
}

QVariant SysInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return sysInfoTable[index.row()].name;
        case 1:
            return sysInfoTable[index.row()].func();
        }
    }

    return QVariant();
}

QVariant SysInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Key");
        case 1:
            return tr("Value");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
