/*
  baseprobetest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef BASEPROBETEST_H
#define BASEPROBETEST_H

#include <config-gammaray.h>

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/paths.h>

#include <QObject>
#include <QTest>

using namespace GammaRay;

class BaseProbeTest : public QObject
{
    Q_OBJECT
public:
    explicit BaseProbeTest(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

protected:
    virtual void createProbe()
    {

        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        // NOLINTNEXTLINE clang-analyzer-cplusplus.NewDeleteLeaks
        QTest::qWait(1); // event loop re-entry
    }
};

#endif // BASEPROBETEST_H
