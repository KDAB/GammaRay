/*
  problemmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    connect(m_problemCollector, &ProblemCollector::aboutToAddProblem, this, &ProblemModel::aboutToAddProblem);
    connect(m_problemCollector, &ProblemCollector::problemAdded, this, &ProblemModel::problemAdded);
    connect(m_problemCollector, &ProblemCollector::aboutToRemoveProblems, this, &ProblemModel::aboutToRemoveProblems);
    connect(m_problemCollector, &ProblemCollector::problemsRemoved, this, &ProblemModel::problemsRemoved);
}

ProblemModel::~ProblemModel() = default;

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
            return problem.locations.size() ? problem.locations.front().displayString() : QString();
        }
        break;
    case ObjectModel::ObjectIdRole:
        return QVariant::fromValue(problem.object);
    case ProblemModelRoles::SourceLocationRole:
        return QVariant::fromValue(problem.locations);
    case ProblemModelRoles::SeverityRole:
        return static_cast<int>(problem.severity);
    case ProblemModelRoles::ProblemIdRole:
        return problem.problemId;
    }
    return QVariant();
}

QMap<int, QVariant> ProblemModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(ObjectModel::ObjectIdRole, data(index, ObjectModel::ObjectIdRole));
    d.insert(ProblemModelRoles::SourceLocationRole, data(index, ProblemModelRoles::SourceLocationRole));
    d.insert(ProblemModelRoles::SeverityRole, data(index, ProblemModelRoles::SeverityRole));
    d.insert(ProblemModelRoles::ProblemIdRole, data(index, ProblemModelRoles::ProblemIdRole));
    return d;
}

int ProblemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_problemCollector->problems().size();
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
void GammaRay::ProblemModel::aboutToRemoveProblems(int row, int count)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
}
void GammaRay::ProblemModel::problemAdded()
{
    endInsertRows();
}
void GammaRay::ProblemModel::problemsRemoved()
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
