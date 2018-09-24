/*
  problemmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation problem.

  Copyright (C) 2016-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "problemmodel.h"
#include "../../problemcollector.h"
#include <common/problem.h>

#include <common/tools/problemreporter/problemmodelroles.h>


using namespace GammaRay;

ProblemModel::ProblemModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_problemCollector(ProblemCollector::instance())
{
    connect(m_problemCollector, SIGNAL(aboutToAddProblem(int)), this, SLOT(aboutToAddProblem(int)));
    connect(m_problemCollector, SIGNAL(problemAdded()), this, SLOT(problemAdded()));
    connect(m_problemCollector, SIGNAL(aboutToRemoveProblem(int)), this, SLOT(aboutToRemoveProblem(int)));
    connect(m_problemCollector, SIGNAL(problemRemoved()), this, SLOT(problemRemoved()));
}

ProblemModel::~ProblemModel()
{
}

QVariant ProblemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Problem &problem = m_problemCollector->problems().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0:
                return problem.description;
            case 1:
                return problem.location.displayString();
            }
        case ObjectModel::ObjectIdRole:
            return QVariant::fromValue(problem.object);
        case ProblemModelRoles::SourceLocationRole:
            return QVariant::fromValue(problem.location);
        case ProblemModelRoles::SeverityRole:
            return problem.severity;
    }
    return QVariant();
}

QMap<int, QVariant> ProblemModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(ObjectModel::ObjectIdRole, data(index, ObjectModel::ObjectIdRole));
    d.insert(ProblemModelRoles::SourceLocationRole, data(index, ProblemModelRoles::SourceLocationRole));
    d.insert(ProblemModelRoles::SeverityRole, data(index, ProblemModelRoles::SeverityRole));
    return d;
}

int ProblemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_problemCollector->problems().count();
}
int ProblemModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

void GammaRay::ProblemModel::aboutToAddProblem(int row)
{
    beginInsertRows(QModelIndex(), row, row);
}
void GammaRay::ProblemModel::aboutToRemoveProblem(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
}
void GammaRay::ProblemModel::problemAdded()
{
    endInsertRows();
}
void GammaRay::ProblemModel::problemRemoved()
{
    endRemoveRows();
}


/*
ClientProblemSelectionModel::ClientProblemSelectionModel(ProblemCollector *manager)
    : QItemSelectionModel(manager->model())
    , m_problemCollector(manager)
{
    connect(manager, SIGNAL(problemSelectedByIndex(int)), this, SLOT(selectProblem(int)));
    connect(manager, SIGNAL(problemListAvailable()), this, SLOT(selectDefaultProblem()));
}

ClientProblemSelectionModel::~ClientProblemSelectionModel()
{
}

void ClientProblemSelectionModel::selectProblem(int index)
{
    select(model()->index(index, 0), QItemSelectionModel::Select
           | QItemSelectionModel::Clear
           | QItemSelectionModel::Rows
           | QItemSelectionModel::Current);
}

void ClientProblemSelectionModel::selectDefaultProblem()
{
    selectProblem(m_problemCollector->problemIndexForProblemId(QStringLiteral("GammaRay::ObjectInspector")));
}*/
