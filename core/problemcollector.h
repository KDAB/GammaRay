/*
  problemcollector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

namespace GammaRay {

class ProblemModel;

class GAMMARAY_CORE_EXPORT ProblemCollector : public QObject
{
    Q_OBJECT

public:
    static void addProblem(const Problem &problem);
    static void removeProblem(const QString &problemId);
    static ProblemCollector *instance();

    const QVector<Problem> &problems();

signals:
    void aboutToAddProblem(int row);
    void problemAdded();
    void aboutToRemoveProblem(int row);
    void problemRemoved();

private:
    explicit ProblemCollector(QObject *parent);

    QVector<Problem> m_problems;

    friend class Probe;
};
}

#endif // GAMMARAY_PROBLEMCOLLECTOR_H

