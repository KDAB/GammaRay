/*
  processfiltermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "processfiltermodel.h"
#include "processmodel.h"

#include <QCoreApplication>

#if defined(Q_OS_WIN)
#include <qt_windows.h>
#include <lmcons.h>
static QString qGetLogin()
{
    char winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
    DWORD winUserNameSize = sizeof(winUserName);
    GetUserNameA(winUserName, &winUserNameSize);
    return QString::fromLocal8Bit(winUserName);
}

#else
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
static QString qGetLogin()
{
    struct passwd *pw = getpwuid(getuid());
    if (!pw || !pw->pw_name)
        return QString();
    return QString::fromLocal8Bit(pw->pw_name);
}

#endif

using namespace GammaRay;

ProcessFilterModel::ProcessFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_currentProcId = QString::number(qApp->applicationPid());
    m_currentUser = qGetLogin();
#ifndef Q_OS_WIN
    if (m_currentUser == QLatin1String("root")) {
        // empty current user == no filter. as root we want to show all
        m_currentUser.clear();
    }
#endif
}

bool ProcessFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const QString l = sourceModel()->data(left).toString();
    const QString r = sourceModel()->data(right).toString();
    if (left.column() == ProcessModel::PIDColumn)
        return l.toInt() < r.toInt();

    return l.compare(r, Qt::CaseInsensitive) <= 0;
}

bool ProcessFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    ProcessModel *source = dynamic_cast<ProcessModel *>(sourceModel());
    if (!source)
        return true;

    const ProcData &data = source->dataForRow(source_row);

    if (!data.abi.isValid())
        return false;

    // hide ourselves as well as the process that launched us
    if (data.ppid == m_currentProcId || data.name.startsWith(QLatin1String("gammaray"), Qt::CaseInsensitive))
        return false;

    if (!m_currentUser.isEmpty() && data.user != m_currentUser)
        return false;

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool ProcessFilterModel::filterAcceptsColumn(int source_column,
                                             const QModelIndex & /*source_parent*/) const
{
    // hide user row if current user was found
    return m_currentUser.isEmpty() || source_column != ProcessModel::UserColumn;
}
