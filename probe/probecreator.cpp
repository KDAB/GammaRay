/*
  probecreator.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include "probecreator.h"

#include <common/endpoint.h>
#include <core/probe.h>

#include <QCoreApplication>
#include <QMetaObject>
#include <QVector>
#include <QThread>

#include <iostream>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

ProbeCreator::ProbeCreator(CreateFlags flags)
    : m_flags(flags)
{
    // push object into the main thread, as windows creates a
    // different thread where this runs in
    moveToThread(QCoreApplication::instance()->thread());
    // delay to foreground thread
    QMetaObject::invokeMethod(this, "createProbe", Qt::QueuedConnection);

    // don't propagate the probe to child processes
    if (qgetenv("GAMMARAY_UNSET_PRELOAD") == "1")
        qputenv("LD_PRELOAD", "");
    if (qgetenv("GAMMARAY_UNSET_DYLD") == "1")
        qputenv("DYLD_INSERT_LIBRARIES", "");

    // HACK the webinspector plugin does this as well, but if the web view is created
    // too early the env var from there isn't going to reach the web process
    qputenv("QTWEBKIT_INSPECTOR_SERVER",
            QByteArray(GAMMARAY_DEFAULT_ANY_ADDRESS) + ':'
                + QByteArray::number(Endpoint::defaultPort() + 1));
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING",
            QByteArray(GAMMARAY_DEFAULT_ANY_ADDRESS) + ':'
                + QByteArray::number(Endpoint::defaultPort() + 1));
}

void ProbeCreator::createProbe()
{
    if (!qApp) {
        deleteLater();
        return;
    }

    // make sure we are in the ui thread
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    if (Probe::isInitialized()) {
        // already exists, so we are trying to re-attach to an already injected process
        if (m_flags & ResendServerAddress) {
            printf("Resending server address...\n");
            Probe::instance()->resendServerAddress();
        }
        deleteLater();
        return;
    }

    Probe::createProbe(m_flags & FindExistingObjects);
    Q_ASSERT(Probe::isInitialized());

    deleteLater();
}
