/*
  processmodel.h

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

#ifndef GAMMARAY_PROCESSMODEL_H
#define GAMMARAY_PROCESSMODEL_H

#include <QAbstractTableModel>

#include "processlist.h"

namespace GammaRay {

class ProcessModel : public QAbstractTableModel
{
  public:
    explicit ProcessModel(QObject *parent = 0);
    virtual ~ProcessModel();

    void setProcesses(const ProcDataList &processes);
    void mergeProcesses(const ProcDataList &processes);
    ProcData dataForIndex(const QModelIndex &index) const;
    ProcData dataForRow(int row) const;
    QModelIndex indexForPid(const QString &pid) const;

    void clear();

    enum Columns {
      PIDColumn,
      NameColumn,
      StateColumn,
      UserColumn,
      COLUMN_COUNT
    };
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    enum CustomRoles {
      PIDRole = Qt::UserRole,
      NameRole,
      StateRole,
      UserRole
    };
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
private:
    ProcDataList m_data;
};

}

#endif // GAMMARAY_PROCESSMODEL_H
