/*
  helpcontroller.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "helpcontroller.h"

#include <common/paths.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QLibraryInfo>
#include <QStandardPaths>

using namespace GammaRay;

namespace GammaRay {
struct HelpControllerPrivate
{
    HelpControllerPrivate() = default;

    void startProcess();
    void sendCommand(const QByteArray &cmd) const;

    QString assistantPath;
    QString qhcPath;
    QProcess *proc = nullptr;
};
}

void HelpControllerPrivate::startProcess()
{
    if (proc)
        return;

    proc = new QProcess(QCoreApplication::instance());
    proc->setProcessChannelMode(QProcess::ForwardedChannels);
    QObject::connect(proc,
                     static_cast<void (QProcess::*)(int,
                                                    QProcess::ExitStatus)>(&QProcess::finished),
                     proc,
                     [this]() {
                         proc->deleteLater();
                         proc = nullptr;
                     });
    proc->setProgram(assistantPath);
    proc->setArguments(QStringList()
                       << QLatin1String("-collectionFile")
                       << qhcPath
                       << QLatin1String("-enableRemoteControl"));
    proc->start();
    proc->waitForStarted();
    sendCommand("expandToc 2;");
}

void HelpControllerPrivate::sendCommand(const QByteArray &cmd) const
{
    if (!proc) {
        return;
    }
    proc->write(cmd);
}

Q_GLOBAL_STATIC(HelpControllerPrivate, s_helpController)

static QString assistantExecutableName()
{
#ifdef Q_OS_OSX
    return QStringLiteral("Assistant.app");
#else
    return QStringLiteral("assistant");
#endif
}

bool HelpController::isAvailable()
{
    auto d = s_helpController();
    if (!d->assistantPath.isEmpty() && !d->qhcPath.isEmpty())
        return true;

    d->assistantPath = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator() + assistantExecutableName();
    QFileInfo assistFile(d->assistantPath);
    if (!assistFile.isExecutable()) {
        qDebug() << "Qt Assistant not found in QT_INSTALL_BINS. Looking in standard Path next.";
        d->assistantPath = QStandardPaths::findExecutable(assistantExecutableName());
        if (d->assistantPath.isEmpty()) {
            qDebug() << "Qt Assistant not found, help not available.";
            return false;
        }
    }

    const QString qhcPath = Paths::documentationPath() + QLatin1String("/gammaray.qhc");
    if (QFileInfo::exists(qhcPath)) {
        d->qhcPath = qhcPath;
        return true;
    } else {
        qDebug() << "gammaray.qhc not found in" << Paths::documentationPath()
                 << " - help not available";
    }
    return false;
}

void HelpController::openContents()
{
    Q_ASSERT(isAvailable());
    auto d = s_helpController();
    d->startProcess();
    d->sendCommand(
        "setSource qthelp://com.kdab.GammaRay." GAMMARAY_PLUGIN_VERSION "/gammaray/index.html;syncContents\n");
}

void HelpController::openPage(const QString &page)
{
    Q_ASSERT(isAvailable());
    auto d = s_helpController();
    d->startProcess();
    d->sendCommand(QByteArray(
                       "setSource qthelp://com.kdab.GammaRay." GAMMARAY_PLUGIN_VERSION "/")
                   + page.toUtf8()
                   + ";syncContents\n");
}
