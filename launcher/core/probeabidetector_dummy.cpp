/*
  probeabidetector_dummy.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "probeabidetector.h"
#include "probeabi.h"

#include <QString>

using namespace GammaRay;

QString ProbeABIDetector::qtCoreForExecutable(const QString &path)
{
    Q_UNUSED(path);
    return QString();
}

QString ProbeABIDetector::qtCoreForProcess(quint64 pid) const
{
    Q_UNUSED(pid);
    return QString();
}

QVector<ProbeABI> ProbeABIDetector::detectAbiForQtCore(const QString &path)
{
    Q_UNUSED(path);
    return { ProbeABI::fromString(GAMMARAY_PROBE_ABI) };
}
