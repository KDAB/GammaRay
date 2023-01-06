/*
  problemcollector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

ProblemCollector *ProblemCollector::instance()
{
    return Probe::instance()->problemCollector();
}

void ProblemCollector::registerProblemChecker(const QString &id,
                                              const QString &name, const QString &description,
                                              const std::function<void()> &callback, bool enabled)
{
    Checker c = { id, name, description, callback, enabled };
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

void ProblemCollector::addProblem(const Problem &problem)
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
void ProblemCollector::removeProblem(const QString &problemId)
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

const QVector<Problem> &ProblemCollector::problems()
{
    return m_problems;
}

QVector<ProblemCollector::Checker> &ProblemCollector::availableCheckers()
{
    return m_availableCheckers;
}

bool ProblemCollector::isCheckerRegistered(const QString &id) const
{
    return std::any_of(m_availableCheckers.begin(), m_availableCheckers.end(),
                       [&id](const Checker &c) { return c.id == id; });
}
