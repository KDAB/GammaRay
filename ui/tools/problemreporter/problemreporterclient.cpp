/*
  problemreporterclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "problemreporterclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ProblemReporterClient::ProblemReporterClient(QObject *parent)
    : ProblemReporterInterface(parent)
{
}

ProblemReporterClient::~ProblemReporterClient() = default;

void ProblemReporterClient::requestScan()
{
    Endpoint::instance()->invokeObject(objectName(), "requestScan");
}
