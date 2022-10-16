/*
  problem.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

struct Problem
{
    enum Severity
    {
        Info = 1,
        Warning,
        Error
    };
    enum FindingCategory
    {
        Unknown,
        Live, ///< is added and removed as the problem arises/vanishes by the reporting tool
        Scan, ///< is added in response to a scan request and removed by the problem collector in advance to the next scan
        Permanent ///< is valid during the whole application lifetime and not meant to be removed at all
    };

    Problem()
        : severity(Error)
        , findingCategory(Unknown)
    {
    }

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
