/*
  selftest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "selftest.h"
#include "probeabi.h"
#include "probefinder.h"
#include "injector/injectorfactory.h"

#include <QFileInfo>

using namespace GammaRay;

SelfTest::SelfTest(QObject *parent)
    : QObject(parent)
{
}

SelfTest::~SelfTest() = default;

bool SelfTest::checkEverything()
{
    auto result = checkProbes();
    result |= checkInjectors();
    return result;
}

bool SelfTest::checkProbes()
{
    int validProbeCount = 0;
    const QVector<ProbeABI> probeABIs = ProbeFinder::listProbeABIs();
    for (const ProbeABI &abi : probeABIs) {
        const QString probePath = ProbeFinder::findProbe(abi);
        if (probePath.isEmpty()) {
            emit error(tr("No probe found for ABI %1.").arg(abi.id()));
            continue;
        }

        QFileInfo fi(probePath);
        if (!fi.exists() || !fi.isFile() || !fi.isReadable()) {
            emit error(tr("Probe at %1 is invalid."));
            continue;
        }

        // TODO: we could do some more elaborate check for plugins etc here?

        emit information(tr("Found valid probe for ABI %1 at %2.").arg(abi.id(), probePath));
        ++validProbeCount;
    }

    if (validProbeCount == 0)
        emit error(tr("No probes found - GammaRay not functional."));
    return validProbeCount;
}

bool SelfTest::checkInjectors()
{
    const auto injectors = InjectorFactory::availableInjectors();
    int viableInjectors = 0;
    for (const auto &injector : injectors) {
        if (checkInjector(injector))
            ++viableInjectors;
    }

    if (!viableInjectors) {
        emit error(tr("No injectors available - GammaRay not functional."));
        return false;
    }
    return true;
}

bool SelfTest::checkInjector(const QString &injectorType)
{
    AbstractInjector::Ptr injector = InjectorFactory::createInjector(injectorType);
    if (!injector) {
        emit error(tr("Unable to create instance of injector %1.").arg(injectorType));
        return false;
    }
    if (!injector->selfTest()) {
        emit error(tr("Injector %1 failed to pass its self-test: %2.").arg(injectorType,
                                                                           injector->errorString()));
        return false;
    }

    emit information(tr("Injector %1 successfully passed its self-test.").arg(injectorType));
    return true;
}
