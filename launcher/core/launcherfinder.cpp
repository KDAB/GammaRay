/*
  launcherfinder.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "launcherfinder.h"
#include "probeabi.h"

#include <config-gammaray.h>
#include <common/paths.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

using namespace GammaRay;

const char *executableNames[] = {
    "gammaray", // the Injector
    "gammaray-launcher", // the LauncherUI
    "gammaray-client" // the Client
};

QString LauncherFinder::findLauncher(LauncherFinder::Type type)
{
    QString fileName = executableNames[type];
#ifdef Q_OS_WIN
    fileName += ".exe";
#endif

    QStringList appPaths; // a list of all the paths we have searched

    QString appPath = Paths::binPath() + QDir::separator() + fileName;
    QFileInfo fi(appPath);
    if (fi.isExecutable())
        return fi.absoluteFilePath();
    appPaths.append(appPath);

    appPath = Paths::libexecPath() + QDir::separator() + fileName;
    if (!appPaths.contains(appPath)) {
        fi.setFile(appPath);
        if (fi.isExecutable())
            return fi.absoluteFilePath();
        appPaths.append(appPath);
    }

    qWarning() << fileName << "not found in the expected location(s):";
    qWarning() << appPaths.join(QStringLiteral(", ")) << Qt::endl
               << "continuing anyway, hoping for it to be in PATH.";
    qWarning() << "This is likely a setup problem." << Qt::endl;
    return fileName;
}
