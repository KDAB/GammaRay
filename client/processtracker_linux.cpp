/*
  processtracker_linux.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

// A said crossplatform way (using ptrace) is available at
// https://forum.juce.com/t/detecting-if-a-process-is-being-run-under-a-debugger/2098

#include "processtracker_linux.h"

#include <QFile>
#include <QStringList>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringDecoder>
#else
#include <QTextCodec>
#endif

namespace {
static QString readFile(const QString &filePath, const QByteArray &codec = QByteArrayLiteral("UTF-8"))
{
    QFile file(filePath);

    if (!file.exists()) {
        qWarning("%s: File does not exists", Q_FUNC_INFO);
        return QString();
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("%s: File can not be opened", Q_FUNC_INFO);
        return QString();
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    return QStringDecoder(codec).decode(file.readAll());
#else
    QTextCodec *tc = QTextCodec::codecForName(codec);
    if (!tc) {
        tc = QTextCodec::codecForLocale();
    }

    return tc->toUnicode(file.readAll());
#endif
}
}

using namespace GammaRay;

ProcessTrackerBackendLinux::ProcessTrackerBackendLinux(QObject *parent)
    : GammaRay::ProcessTrackerBackend(parent)
{
}

void ProcessTrackerBackendLinux::checkProcess(qint64 pid)
{
    GammaRay::ProcessTrackerInfo pinfo(pid);
    QString buffer(readFile(QString::fromLatin1("/proc/%1/status").arg(pid)));

    if (!buffer.isEmpty()) {
        const QStringList values = buffer.remove(QLatin1Char('\t')).split(QLatin1Char('\n'));

        for (const QString &value : values) {
            if (value.startsWith(QStringLiteral("TracerPid:"))) {
                pinfo.traced = value.split(QLatin1Char(':')).value(1).simplified().toLongLong();
            }
            else if (value.startsWith(QStringLiteral("State:"))) {
                const char status(value.split(QLatin1Char(':')).value(1).simplified()[0].toUpper().toLatin1());

                // status decoding as taken from fs/proc/array.c
                switch (status) {
                    case 'T': {
                        pinfo.state = GammaRay::ProcessTracker::Suspended;
                        break;
                    }

                    case 'S': // Sleeping
                    case 'R': {
                        pinfo.state = GammaRay::ProcessTracker::Running;
                        break;
                    }

                    //case 'Z': // Zombie
                    //case 'D': // Disk Sleep
                    //case 'W': // Paging
                }
            }
        }
    }

    emit processChecked(pinfo);
}
