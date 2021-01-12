/*
  libraryinfomodel.cpp

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

#include "libraryinfomodel.h"

#include <QLibraryInfo>

using namespace GammaRay;

struct location_info_t {
    QLibraryInfo::LibraryLocation loc;
    const char* name;
};

#define L(x) { QLibraryInfo:: x, #x }
static const location_info_t locInfoTable[] = {
    L(PrefixPath),
    L(DocumentationPath),
    L(HeadersPath),
    L(LibrariesPath),
    L(LibraryExecutablesPath),
    L(BinariesPath),
    L(PluginsPath),
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    L(ImportsPath),
#endif
    L(Qml2ImportsPath),
    L(ArchDataPath),
    L(DataPath),
    L(TranslationsPath),
    L(ExamplesPath),
    L(TestsPath),
    L(SettingsPath)
};
#undef L
static const auto locInfoTableSize = sizeof(locInfoTable) / sizeof(location_info_t);

LibraryInfoModel::LibraryInfoModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int LibraryInfoModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int LibraryInfoModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return locInfoTableSize;
}

QVariant LibraryInfoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return locInfoTable[index.row()].name;
            case 1: return QLibraryInfo::location(locInfoTable[index.row()].loc);
        }
    }

    return QVariant();
}

QVariant LibraryInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Location");
            case 1:
                return tr("Path");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
