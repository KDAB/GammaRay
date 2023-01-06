/*
  libraryutil.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "libraryutil.h"

using namespace GammaRay;

#include <QByteArray>
#include <QDebug>
#include <QProcess>

static QVector<QByteArray> dependenciesInternal(const QString &fileName, bool fallback = false)
{
    QVector<QByteArray> result;

    QProcess proc;
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.setReadChannel(QProcess::StandardOutput);
    if (!fallback) {
        // first try to use ldd
        proc.start(QStringLiteral("ldd"), QStringList() << fileName);
        if (!proc.waitForStarted()) // if that is not available, run the fallback
            return dependenciesInternal(fileName, true);
    } else {
        // see https://man7.org/linux/man-pages/man8/ld.so.8.html
        // by setting LD_TRACE_LOADED_OBJECTS=1 we make ld.so behave like ldd
        // this works even on embedded systems where ldd is not available
        QProcessEnvironment env = proc.processEnvironment();
        env.insert(QStringLiteral("LD_TRACE_LOADED_OBJECTS"), QStringLiteral("1"));
        proc.setProcessEnvironment(env);
        proc.start(fileName);
    }
    proc.waitForFinished();

    while (proc.canReadLine()) {
        const QByteArray line = proc.readLine();
        if (line.isEmpty())
            break;

        const int begin = line.indexOf("=> ");
        const int end = line.lastIndexOf(" (");
        if (begin <= 0 || end <= 0 || end <= begin) {
            continue;
        }
        result.push_back(line.mid(begin + 3, end - begin - 3).trimmed());
    }

    return result;
}

QVector<QByteArray> LibraryUtil::dependencies(const QString &fileName)
{
    return dependenciesInternal(fileName);
}
