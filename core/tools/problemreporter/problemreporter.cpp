/*
  problemreporter.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
