/*
  problemreporter.cpp

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

#include "problemreporter.h"
#include "problemmodel.h"
#include "availablecheckersmodel.h"

#include <core/problemcollector.h>

using namespace GammaRay;

ProblemReporter::ProblemReporter(Probe *probe, QObject *parent)
    : ProblemReporterInterface(parent)
    , m_problemModel(new ProblemModel(this))
{
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ProblemModel"), m_problemModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.AvailableProblemCheckersModel"), new AvailableCheckersModel(this));

    connect(ProblemCollector::instance(), &ProblemCollector::problemScansFinished, this, &ProblemReporterInterface::problemScansFinished);
}

ProblemReporter::~ProblemReporter() = default;

void GammaRay::ProblemReporter::requestScan()
{
    ProblemCollector::instance()->requestScan();
}
