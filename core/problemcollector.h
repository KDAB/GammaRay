/*
  problemcollector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROBLEMCOLLECTOR_H
#define GAMMARAY_PROBLEMCOLLECTOR_H

// Own
#include "gammaray_core_export.h"

#include <common/problem.h>
#include <common/objectid.h>
#include <common/sourcelocation.h>

// Qt
#include <QAbstractItemModel>

// Std
#include <memory>
#include <vector>
#include <functional>

namespace GammaRay {

class ProblemModel;

class GAMMARAY_CORE_EXPORT ProblemCollector : public QObject
{
    Q_OBJECT

public:
    static void addProblem(const Problem &problem);

    /**
     * Removes the problem with the \p problemId. It's ok to call this method
     * with a problemId that is not registered, in this case nothing happens.
     *
     * It's not required to call this for problems with findingCategory Scan,
     * as those are removed automatically in advance to a new scan.
     */
    static void removeProblem(const QString &problemId);
    static ProblemCollector *instance();

    const QVector<Problem> &problems();

    /**
     * Use this method from a tool to register a scan, which the tool is able
     * to do.
     *
     * \p name and \p description are user-readable strings, \p id is some
     * internal string that will be used to identify the checkers. This can be
     * chosen at will, but must be unique.
     * \p callback will be called to start the scan
     */
    static void registerProblemChecker(const QString &id,
                                    const QString &name, const QString &description,
                                    const std::function<void()> &callback,
                                    bool enabled = true);

    /// Meant to be used in unit tests
    bool isCheckerRegistered(const QString &id) const;

private:
    struct Checker {
        QString id;
        QString name;
        QString description;
        std::function<void()> callback;
        bool enabled;
    };
    QVector<Checker> &availableCheckers();

signals:
    /**
     * These signals are directed at the problem model to inform about changes
     * in the result set.
     */
    void aboutToAddProblem(int row);
    void problemAdded();
    void aboutToRemoveProblems(int first, int count = 1);
    void problemsRemoved();

    /**
     * This signal is directed at the Problem Reporter tool to inform that
     * the problem providing tools have started scanning for problems.
     */
    void problemScansFinished();

    /**
     * These signals are directed at the available checkers model to inform newly
     * available checkers
     */
    void aboutToAddChecker();
    void checkerAdded();

public slots:
    void requestScan();

private:
    explicit ProblemCollector(QObject *parent);
    void clearScans();

    QVector<Checker> m_availableCheckers;
    QVector<Problem> m_problems;

    friend class Probe;
    friend class AvailableCheckersModel;
    friend class ProblemReporterTest;
};
}

#endif // GAMMARAY_PROBLEMCOLLECTOR_H

