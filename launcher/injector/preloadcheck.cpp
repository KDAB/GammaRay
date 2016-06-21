/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include <config-gammaray.h>
#include "preloadcheck.h"

#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)

#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QProcess>

using namespace GammaRay;

PreloadCheck::PreloadCheck()
{
}

bool PreloadCheck::test(const QString &fileName, const QString &symbol)
{
    if (fileName.isEmpty()) {
        setErrorString(tr("Cannot find file containing symbol: %1").arg(symbol));
        return false;
    }

    if (!QFile(fileName).exists()) {
        setErrorString(tr("Invalid shared object: %1").arg(fileName));
        return false;
    }

    QStringList args;
    args << QStringLiteral("--relocs") << QStringLiteral("--wide") << fileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(QStringLiteral("readelf"), args, QIODevice::ReadOnly);
    if (!proc.waitForFinished()) {
        // TODO: Find out if we want to error out if 'readelf' is missing
        // The question is: Do all (major) distributions ship binutils by default?
        // Major distros do, but not custom embedded ones...
        setErrorString(tr("Failed to run 'readelf' (binutils) binary: %1").
                       arg(QString(proc.errorString())));
        return true;
    }

    if (proc.exitCode() != 0) {
        setErrorString(tr("Cannot read shared object: %1").arg(QString(proc.readAll())));
        return false;
    }

    // Example line on x86_64:
    // 00000049f3d8  054300000007 R_X86_64_JUMP_SLO 000000000016c930 qt_startup_hook + 0
    // Example line on i386:
    // 002e02f0  00034407 R_386_JUMP_SLOT        00181490   qt_startup_hook
    QRegExp rx("^(?:[^ ]+\\s+){4}([^ ]+)(?:.*)$");
    while (proc.canReadLine()) {
        const QString line = proc.readLine().trimmed();
        if (!rx.exactMatch(line))
            continue;

        const QString currentSymbol = rx.cap(1);
        if (currentSymbol == symbol) {
            qDebug() << "Found relocatable symbol in" << fileName << ":" << symbol;
            setErrorString(QString());
            return true;
        }
    }

#if defined(__mips__) && defined(GAMMARAY_ENABLE_GPL_ONLY_FEATURES)
    // Mips, besides the plt, has another method of
    // calling functions from .so files, and this method doesn't need JUMP_SLOT
    // relocations (in fact, it doesn't need any relocations). This method uses .got
    // entries and lazy binding stubs.
    if (testMips(symbol, fileName)) {
        qDebug() << "Call of function " << symbol << " will go through lazy binding stub";
        setErrorString(QString());
        return true;
    }
#endif

    setErrorString(tr("Symbol is not marked as relocatable: %1").arg(symbol));
    return false;
}

QString PreloadCheck::errorString() const
{
    return m_errorString;
}

void PreloadCheck::setErrorString(const QString &err)
{
    m_errorString = err;
}

#ifdef GAMMARAY_ENABLE_GPL_ONLY_FEATURES
#include "preloadcheck_mips.cpp"
#endif

#endif
