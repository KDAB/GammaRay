/*
  problemmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_PROBLEMMODEL_H
#define GAMMARAY_PROBLEMMODEL_H

#include <QAbstractListModel>
#include <QItemSelectionModel>

#include <common/objectmodel.h>

namespace GammaRay {

class ProblemCollector;

/*! Model of all selectable client tools. */
class ProblemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProblemModel(QObject *parent);
    ~ProblemModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;

private slots:
    void aboutToAddProblem(int row);
    void problemAdded();
    void aboutToRemoveProblems(int row, int count = 1);
    void problemsRemoved();

private:
    ProblemCollector *m_problemCollector;
};

/*! Selection model that automatically syncs ClientToolModel with ClientToolManager. */
// class ClientToolSelectionModel : public QItemSelectionModel
// {
//     Q_OBJECT
// public:
//     explicit ClientToolSelectionModel(ClientToolManager *manager);
//     ~ClientToolSelectionModel();
//
// private slots:
//     void selectTool(int index);
//     void selectDefaultTool();
//
// private:
//     ClientToolManager *m_toolManager;
// };

}

#endif // GAMMARAY_PROBLEMMODEL_H
