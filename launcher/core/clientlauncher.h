/*
  clientlauncher.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTLAUNCHER_H
#define GAMMARAY_CLIENTLAUNCHER_H

#include "gammaray_launcher_export.h"

#include <QProcess>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

namespace GammaRay {
/** Launching/monitoring of the GammaRay client for out-of-process use. */
class GAMMARAY_LAUNCHER_EXPORT ClientLauncher
{
public:
    ClientLauncher();
    ~ClientLauncher();

    bool launch(const QUrl &url);
    void terminate();
    void waitForFinished();

    static void launchDetached(const QUrl &url);

private:
    Q_DISABLE_COPY(ClientLauncher)
    static QString clientPath();
    static QStringList makeArgs(const QUrl &url);

private:
    QProcess m_process;
};
}

#endif // GAMMARAY_CLIENTLAUNCHER_H
