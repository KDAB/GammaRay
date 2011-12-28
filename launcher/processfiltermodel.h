/*
  processfiltermodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_PROCESSFILTERMODEL_H
#define GAMMARAY_PROCESSFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {

// A filterable and sortable process model
class ProcessFilterModel : public QSortFilterProxyModel
{
  public:
    explicit ProcessFilterModel(QObject *parent);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;

  private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    QString m_currentProcId;
    QString m_currentUser;
};

}

#endif // GAMMARAY_PROCESSFILTERMODEL_H
