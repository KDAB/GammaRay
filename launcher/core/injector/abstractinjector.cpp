/*
  abstractinjector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "abstractinjector.h"

#include <QDebug>
#include <QDir>

using namespace GammaRay;

AbstractInjector::~AbstractInjector() = default;

bool AbstractInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                              const QString &probeFunc, const QProcessEnvironment &env)
{
    Q_UNUSED(programAndArgs);
    Q_UNUSED(probeDll);
    Q_UNUSED(probeFunc);
    Q_UNUSED(env);
    qWarning() << "Injection on launch not supported by this injector.";
    return false;
}

bool AbstractInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
    Q_UNUSED(pid);
    Q_UNUSED(probeDll);
    Q_UNUSED(probeFunc);
    qWarning() << "Attaching to a running process is not supported by this injector.";
    return false;
}

bool AbstractInjector::selfTest()
{
    return true;
}

QString AbstractInjector::workingDirectory() const
{
    if (m_workingDir.isEmpty())
        return QDir::currentPath();
    return m_workingDir;
}

void AbstractInjector::setWorkingDirectory(const QString &path)
{
    m_workingDir = path;
}

ProbeABI AbstractInjector::targetAbi() const
{
    return m_targetAbi;
}

void AbstractInjector::setTargetAbi(const ProbeABI &abi)
{
    m_targetAbi = abi;
}
