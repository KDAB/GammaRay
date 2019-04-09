/*
  modelcellmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELINSPECTOR_MODELCELLMODEL_H
#define GAMMARAY_MODELINSPECTOR_MODELCELLMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {
class ModelCellModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelCellModel(QObject *parent = nullptr);
    void setModelIndex(const QModelIndex &idx);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    typedef QPair<int, QString> RoleInfo;
    static QVector<RoleInfo> rolesForModel(const QAbstractItemModel *model);

    QPersistentModelIndex m_index;
    QVector<RoleInfo> m_roles;
};
}

#endif // GAMMARAY_MODELCELLMODEL_H
