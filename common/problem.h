/*
  problem.h

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

#ifndef GAMMARAY_PROBLEM_H
#define GAMMARAY_PROBLEM_H

// Own
#include "objectid.h"
#include "common/sourcelocation.h"

// Qt
#include <QAbstractItemModel>

// Std
#include <memory>
#include <vector>

namespace GammaRay {

struct Problem {
    enum Severity {
        Info = 1,
        Warning,
        Error
    };
    enum FindingCategory {
        Unknown,
        Live, ///< is added and removed as the problem arises/vanishes by the reporting tool
        Scan, ///< is added in response to a scan request and removed by the problem collector in advance to the next scan
        Permanent ///< is valid during the whole application lifetime and not meant to be removed at all
    };

    Problem()
        : severity(Error)
        , findingCategory(Unknown)
    {}

    Severity severity;
    ObjectId object;
    QString description;
    QVector<SourceLocation> locations;
    QString reportingTool; ///< Tool which reported the issue
    QString problemId; ///< tool-specific unique id for the problem
    FindingCategory findingCategory;

    bool operator==(const Problem &other) const
    {
        return problemId == other.problemId;
    }
};

}

Q_DECLARE_METATYPE(GammaRay::Problem::Severity)

#endif // GAMMARAY_PROBLEM_H

