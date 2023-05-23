/*
  attachhelper.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "attachhelper.h"

#ifdef Q_OS_WIN32
#ifndef NOMINMAX
// compile fix for Qt5+VS2010+QDateTime, see:
// https://forum.qt.io/topic/21605/solved-qt5-vs2010-qdatetime-not-enough-actual-parameters-for-macro-min-max
#define NOMINMAX
#endif

#include <qt_windows.h>
#endif

#include <QCoreApplication>
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QDateTime>

#include <cstdlib>

AttachHelper::AttachHelper(const QString &gammaray, const QString &injector,
                           const QString &debuggee, const QStringList &arguments, QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_proc(new QProcess(this))
    , m_gammaray(gammaray)
    , m_injector(injector)
{
    m_proc->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(m_proc, &QProcess::started, this, &AttachHelper::processStarted);
    connect(m_proc, &QProcess::finished, this, &AttachHelper::processFinished);
    m_proc->start(debuggee, arguments);
}

void AttachHelper::processStarted()
{
    // attach randomly after 1-1500 ms
    std::srand(QDateTime::currentMSecsSinceEpoch());
    const int timeout = std::rand() % 1500 + 1;
    qDebug() << "attaching gammaray in" << timeout << "ms";
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &AttachHelper::attach);
    m_timer->start(timeout);
}

void AttachHelper::processFinished(int exitCode)
{
    qApp->exit(exitCode);
}

void AttachHelper::attach()
{
    if (m_proc->state() != QProcess::Running)
        return;

    qDebug() << "attaching gammaray";
    QProcess gammaray;
    gammaray.setProcessChannelMode(QProcess::ForwardedChannels);
    QStringList args;
    args << QStringLiteral("--inprocess") << QStringLiteral("-i") << m_injector;
    args << QStringLiteral("-p") << QString::number(m_proc->processId());
    args << QStringLiteral("-nodialogs");
    args << QStringLiteral("--listen") << QStringLiteral("tcp://127.0.0.1/");
    const int ret = gammaray.execute(m_gammaray, args);
    if (ret != 0) {
        m_proc->kill();
        qFatal("could not attach to debuggee");
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().size() < 4) {
        qWarning() << "usage: " << app.applicationName()
                   << " GAMMARAY INJECTOR DEBUGGEE [DEBUGGEE_ARGS]";
        return 1;
    }

    QStringList args = app.arguments();
    // remove path to this bin
    args.removeFirst();
    // gammaray
    const QString gammaray = args.takeFirst();
    // injector
    const QString injector = args.takeFirst();
    // app to run
    const QString debuggee = args.takeFirst();

    // run the self-test first, and skip the test if that fails
    // this prevents failures with Yama ptrace_scope activated for example
    if (QProcess::execute(gammaray,
                          QStringList() << QStringLiteral("--self-test") << injector)
        == 1) {
        qWarning() << "Skipping test due to injector self-test failure!";
        return 0;
    }

    AttachHelper helper(gammaray, injector, debuggee, args);

    return app.exec();
}
