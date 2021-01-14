/*
  sysinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sysinfomodel.h"

#include <QLibraryInfo>
#include <QSysInfo>

using namespace GammaRay;

struct sysinfo_t {
    QString(*func)();
    const char* name;
};

#define S(x) { QSysInfo:: x, #x }
static const sysinfo_t sysInfoTable[] = {
#if !defined(Q_CC_MSVC) || _MSC_VER > 1600 //krazy:exclude=cpp to deal with older MS compilers
    { []() { return QString::fromLatin1(QLibraryInfo::build()); }, "build" },
#endif
    S(buildAbi),
    S(buildCpuArchitecture),
    S(currentCpuArchitecture),
    S(kernelType),
    S(kernelVersion),
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    S(machineHostName),
#endif
    S(prettyProductName),
    S(productType),
    S(productVersion)
};
#undef S
static const auto sysInfoTableSize = sizeof(sysInfoTable) / sizeof(sysinfo_t);

SysInfoModel::SysInfoModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int SysInfoModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int SysInfoModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return sysInfoTableSize;
}

QVariant SysInfoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return sysInfoTable[index.row()].name;
            case 1: return sysInfoTable[index.row()].func();
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
