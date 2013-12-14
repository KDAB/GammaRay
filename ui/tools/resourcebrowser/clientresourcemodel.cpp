/*
  clientresourcemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "clientresourcemodel.h"

#include <QIcon>

using namespace GammaRay;

ClientResourceModel::ClientResourceModel(QObject* parent): KRecursiveFilterProxyModel(parent)
{
}

ClientResourceModel::~ClientResourceModel()
{

}

QVariant ClientResourceModel::data(const QModelIndex& index, int role) const
{
  if (role == Qt::DecorationRole && index.column() == 0) {
    if (!index.parent().isValid())
      return m_iconProvider.icon(QFileIconProvider::Drive);
    if (hasChildren(index))
      return m_iconProvider.icon(QFileIconProvider::Folder);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QList<QMimeType> types = m_mimeDb.mimeTypesForFileName(index.data(Qt::DisplayRole).toString());
    foreach( const QMimeType &mt, types) {
      QIcon icon = QIcon::fromTheme(mt.iconName());
      if (!icon.isNull())
        return icon;
      icon = QIcon::fromTheme(mt.genericIconName());
      if (!icon.isNull())
        return icon;
    }
#endif
    return m_iconProvider.icon(QFileIconProvider::File);
  }
  return QSortFilterProxyModel::data(index, role);
}

