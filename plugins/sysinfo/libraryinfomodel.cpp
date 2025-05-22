/*
  libraryinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "libraryinfomodel.h"

#include <QLibraryInfo>

using namespace GammaRay;

struct location_info_t
{
    QLibraryInfo::LibraryPath loc;
    const char *name;
};

#define L(x)                \
    {                       \
        QLibraryInfo::x, #x \
    }
static const location_info_t locInfoTable[] = {
    L(PrefixPath),
    L(DocumentationPath),
    L(HeadersPath),
    L(LibrariesPath),
    L(LibraryExecutablesPath),
    L(BinariesPath),
    L(PluginsPath),
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

LibraryInfoModel::LibraryInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int LibraryInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int LibraryInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return locInfoTableSize;
}

QVariant LibraryInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return locInfoTable[index.row()].name;
        case 1:
            return QLibraryInfo::path(locInfoTable[index.row()].loc);
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
