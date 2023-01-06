/*
  clientlauncher.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientlauncher.h"
#include "launcherfinder.h"

#include <QUrl>

#include <iostream>

using namespace GammaRay;

ClientLauncher::ClientLauncher()
{
    m_process.setProcessChannelMode(QProcess::ForwardedChannels);
}

ClientLauncher::~ClientLauncher() = default;

QString ClientLauncher::clientPath()
{
    return LauncherFinder::findLauncher(LauncherFinder::Client);
}

QStringList ClientLauncher::makeArgs(const QUrl &url)
{
    QStringList args;
    args.push_back(url.toString());
    return args;
}

bool ClientLauncher::launch(const QUrl &url)
{
    m_process.start(clientPath(), makeArgs(url));
    return m_process.waitForStarted();
}

void ClientLauncher::launchDetached(const QUrl &url)
{
    const auto args = makeArgs(url);
    std::cout << "Detaching: " << qPrintable(clientPath()) << " " << qPrintable(args.join(" ")) << std::endl;
    QProcess::startDetached(clientPath(), args);
}

void ClientLauncher::terminate()
{
    if (m_process.state() == QProcess::Running)
        m_process.terminate();
}

void ClientLauncher::waitForFinished()
{
    if (m_process.state() == QProcess::Running)
        m_process.waitForFinished(-1);
}
