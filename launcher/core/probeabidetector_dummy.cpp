/*
  probeabidetector_dummy.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
