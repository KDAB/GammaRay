/*
  problemreporter.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
