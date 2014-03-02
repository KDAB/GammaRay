/*
  probeabidetectortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "config-gammaray.h"

#include <common/probeabi.h>
#include <common/probeabidetector.h>

#include <QtTest/qtest.h>
#include <QObject>

using namespace GammaRay;

class ProbeABIDetectorTest : public QObject
{
  Q_OBJECT
private slots:
  void testDetectExecutable()
  {
    ProbeABIDetector detector;
    const ProbeABI abi = detector.abiForExecutable(QCoreApplication::applicationFilePath());
    QCOMPARE(abi.id(), QString(GAMMARAY_PROBE_ABI));
  }

  void testDetectProcess()
  {
    ProbeABIDetector detector;
    const ProbeABI abi = detector.abiForProcess(QCoreApplication::applicationPid());
    QCOMPARE(abi.id(), QString(GAMMARAY_PROBE_ABI));
  }

};

QTEST_MAIN(ProbeABIDetectorTest)

#include "probeabidetectortest.moc"