/*
  libraryinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "libraryinfomodel.h"

#include <QLibraryInfo>

using namespace GammaRay;

struct location_info_t
{
    QLibraryInfo::LibraryLocation loc;
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
            return QLibraryInfo::location(locInfoTable[index.row()].loc);
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
