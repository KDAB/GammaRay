/**************************************************************************
**
** This code is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "processlist.h"

#include <launcher/core/probeabidetector.h>

#include <QProcess>
#include <QtConcurrent/QtConcurrentMap>
#include <QDir>

#include <algorithm>

Q_GLOBAL_STATIC(GammaRay::ProbeABIDetector, s_abiDetector)

static bool isUnixProcessId(const QString &procname)
{
    for (int i = 0; i != procname.size(); ++i) {
        if (!procname.at(i).isDigit())
            return false;
    }
    return true;
}

struct PidAndNameMatch
{
    explicit PidAndNameMatch(qint64 ppid, const QString &name)
        : m_ppid(ppid)
        , m_name(name)
    {
    }

    bool operator()(const ProcData &p) const
    {
        return p.ppid == m_ppid && m_name == p.name;
    }

    const qint64 m_ppid;
    const QString m_name;
};

// Determine UNIX processes by running ps
static ProcDataList unixProcessListPS(const ProcDataList &previous)
{
#ifdef Q_OS_MAC
    // command goes last, otherwise it is cut off
    static const char formatC[] = "pid state user command";
#elif defined(Q_OS_FREEBSD)
    // command goes last, otherwise it is cut off
    static const char formatC[] = "pid state user comm";
#else
    static const char formatC[] = "pid,state,user,cmd";
#endif
    ProcDataList rc;
    QProcess psProcess;
    QStringList args;
    args << QStringLiteral("-e") << QStringLiteral("-o") << QLatin1String(formatC);
    psProcess.start(QStringLiteral("ps"), args);
    if (!psProcess.waitForStarted())
        return rc;
    psProcess.waitForFinished();
    QByteArray output = psProcess.readAllStandardOutput();
    // Split "457 S+   /Users/foo.app"
    const QStringList lines = QString::fromLocal8Bit(output).split(QLatin1Char('\n'));
    const int lineCount = ( int )lines.size();
    const QChar blank = QLatin1Char(' ');
    for (int l = 1; l < lineCount; l++) { // Skip header
        const QString line = lines.at(l).simplified();
        // we can't just split on blank as the process name might
        // contain them
        const auto endOfPid = line.indexOf(blank);
        const auto endOfState = line.indexOf(blank, endOfPid + 1);
        const auto endOfUser = line.indexOf(blank, endOfState + 1);
        if (endOfPid >= 0 && endOfState >= 0 && endOfUser >= 0) {
            ProcData procData;
            procData.ppid = line.left(endOfPid).toULongLong();
            procData.state = line.mid(endOfPid + 1, endOfState - endOfPid - 1);
            procData.user = line.mid(endOfState + 1, endOfUser - endOfState - 1);
            procData.name = line.right(line.size() - endOfUser - 1);
            PidAndNameMatch f(procData.ppid, procData.name);
            ProcDataList::ConstIterator it = std::find_if(previous.constBegin(), previous.constEnd(), std::move(f));
            if (it != previous.constEnd())
                procData.abi = it->abi;
            else
                procData.abi = s_abiDetector->abiForProcess(procData.ppid);
            rc.push_back(procData);
        }
    }

    return rc;
}

struct ProcIdToProcData
{
    ProcIdToProcData(const ProcDataList &prev)
        : previous(prev)
    {
    }

    typedef ProcData result_type;

    ProcData operator()(const QString &procId) const
    {
        ProcData proc;
        if (!isUnixProcessId(procId))
            return proc;

        const QString filename = QLatin1String("/proc/") + procId + QLatin1String("/stat");
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
            return proc; // process may have exited

        const QStringList data = QString::fromLocal8Bit(file.readAll()).split(' ');

        proc.ppid = procId.toULongLong();
        proc.name = data.at(1);
        if (proc.name.startsWith(QLatin1Char('(')) && proc.name.endsWith(QLatin1Char(')'))) {
            proc.name.truncate(proc.name.size() - 1);
            proc.name.remove(0, 1);
        }
        proc.state = data.at(2);
        // PPID is element 3

        proc.user = QFileInfo(file).owner();
        file.close();

        QFile cmdFile(QLatin1String("/proc/") + procId + QLatin1String("/cmdline"));
        if (cmdFile.open(QFile::ReadOnly)) {
            QByteArray cmd = cmdFile.readAll();
            cmd.replace('\0', ' ');
            if (!cmd.isEmpty())
                proc.name = QString::fromLocal8Bit(cmd).trimmed();
        }
        cmdFile.close();

        ProcDataList::ConstIterator it = std::find_if(previous.constBegin(), previous.constEnd(),
                                                      PidAndNameMatch(proc.ppid, proc.name));
        if (it != previous.constEnd())
            proc.abi = it->abi;
        else
            proc.abi = s_abiDetector->abiForProcess(proc.ppid);

        return proc;
    }

private:
    const ProcDataList &previous;
};


// Determine UNIX processes by reading "/proc". Default to ps if
// it does not exist
ProcDataList processList(const ProcDataList &previous)
{
    const QDir procDir(QStringLiteral("/proc/"));
#ifndef Q_OS_FREEBSD
    if (!procDir.exists())
#endif
        return unixProcessListPS(previous);

    ProcDataList rc;
    const QStringList procIds = procDir.entryList();
    if (procIds.isEmpty())
        return rc;

    ProcIdToProcData procIdToProcData(previous);

    // start collection
    rc = QtConcurrent::blockingMapped<ProcDataList>(procIds, procIdToProcData);

    // Filter out invalid entries
    rc.erase(std::remove_if(rc.begin(), rc.end(), [](const ProcData &pd) {
                 return pd.ppid == 0;
             }),
             rc.end());

    return rc;
}
