/*
  problemcollector.cpp

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

// Own
#include "problemcollector.h"

#include "probe.h"

#include <compat/qasconst.h>

using namespace GammaRay;

ProblemCollector::ProblemCollector(QObject *parent)
    : QObject(parent)
{
}

ProblemCollector * ProblemCollector::instance()
{
    return Probe::instance()->problemCollector();
}

void ProblemCollector::registerProblemChecker(const QString& id,
                                           const QString& name, const QString& description,
                                           const std::function<void ()>& callback, bool enabled)
{
    Checker c = {id, name, description, callback, enabled};
    instance()->m_availableCheckers.push_back(c);
}

void GammaRay::ProblemCollector::requestScan()
{
    clearScans();

    for (const auto &checker : qAsConst(m_availableCheckers)) {
        if (checker.enabled)
            checker.callback();
    }
    emit problemScansFinished();
}

void ProblemCollector::addProblem(const Problem& problem)
{
    auto self = instance();

    auto i = std::find(self->m_problems.begin(), self->m_problems.end(), problem);
    if (i != self->m_problems.end()) {
        // if an already reported problem is reported a second time, but with a different source location,
        // then the problem involves multiple source locations. So let's keep all of them.
        std::remove_copy_if(problem.locations.begin(), problem.locations.end(), std::back_inserter(i->locations),
                            [&](const SourceLocation &loc) { return i->locations.contains(loc); });
        return;
    }

    emit self->aboutToAddProblem(self->m_problems.size());
    self->m_problems.push_back(problem);
    emit self->problemAdded();
}
void ProblemCollector::removeProblem(const QString& problemId)
{
    auto self = instance();
    auto it = std::find_if(self->m_problems.begin(), self->m_problems.end(), [&](const Problem &problem) { return problem.problemId == problemId; });
    if (it == self->m_problems.end())
        return;
    auto row = std::distance(self->m_problems.begin(), it);

    emit self->aboutToRemoveProblems(row);
    self->m_problems.erase(it);
    emit self->problemsRemoved();
}

void ProblemCollector::clearScans()
{
    // Remove all elements which originate from a previous scan, before doing a new scan
    // and do so, properly informing the model about all changes.
    auto firstToDeleteIt = m_problems.begin();
    auto it = firstToDeleteIt;
    while (true) {
        if (it != m_problems.end() && it->findingCategory == Problem::Scan) {
            ++it;
        } else if (firstToDeleteIt != it) { // this is supposed to be called also if `it == m_problems.end()`
            auto firstRow = std::distance(m_problems.begin(), firstToDeleteIt);
            auto count = std::distance(m_problems.begin(), it) - firstRow;
            emit aboutToRemoveProblems(firstRow, count);
            firstToDeleteIt = it = m_problems.erase(firstToDeleteIt, it);
            emit problemsRemoved();
        } else if (it != m_problems.end()) {
            ++it;
            ++firstToDeleteIt;
        } else {
            break;
        }
    }
}

const QVector<Problem> & ProblemCollector::problems()
{
    return m_problems;
}

QVector<ProblemCollector::Checker> &ProblemCollector::availableCheckers()
{
    return m_availableCheckers;
}

bool ProblemCollector::isCheckerRegistered(const QString& id) const
{
    return std::any_of(m_availableCheckers.begin(), m_availableCheckers.end(),
                        [&id](const Checker &c){ return c.id == id; }
                      );
}


