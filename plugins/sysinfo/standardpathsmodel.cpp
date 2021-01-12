/*
  standardpathsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "standardpathsmodel.h"

#include <QStandardPaths>

using namespace GammaRay;

struct standard_path_t {
    QStandardPaths::StandardLocation location;
    const char *locationName;
};

#define P(x) { QStandardPaths:: x, #x }

static const standard_path_t standard_paths[] = {
    P(DesktopLocation),
    P(DocumentsLocation),
    P(FontsLocation),
    P(ApplicationsLocation),
    P(MusicLocation),
    P(MoviesLocation),
    P(PicturesLocation),
    P(TempLocation),
    P(HomeLocation),
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    P(DataLocation),
#endif
    P(CacheLocation),
    P(GenericDataLocation),
    P(RuntimeLocation),
    P(ConfigLocation),
    P(DownloadLocation),
    P(GenericCacheLocation),
    P(GenericConfigLocation),
    P(AppDataLocation),
    P(AppConfigLocation),
};

#undef P

static const int standard_path_count = sizeof(standard_paths) / sizeof(standard_path_t);

StandardPathsModel::StandardPathsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

StandardPathsModel::~StandardPathsModel() = default;

QVariant StandardPathsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return static_cast<int>(Qt::AlignLeft | Qt::AlignTop);

    if (role == Qt::DisplayRole) {
        const QStandardPaths::StandardLocation loc = standard_paths[index.row()].location;
        switch (index.column()) {
        case 0:
            return QString::fromLatin1(standard_paths[index.row()].locationName);
        case 1:
            return QStandardPaths::displayName(loc);
        case 2:
            return QStandardPaths::standardLocations(loc).join(QLatin1Char('\n'));
        case 3:
            return QStandardPaths::writableLocation(loc);
        }
    }

    return QVariant();
}

int StandardPathsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int StandardPathsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return standard_path_count;
}

QVariant StandardPathsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case 0:
        return tr("Type");
    case 1:
        return tr("Display Name");
    case 2:
        return tr("Standard Locations");
    case 3:
        return tr("Writable Location");
    }
    return QVariant();
}
