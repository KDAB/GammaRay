/*
  kjobmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "kjobmodel.h"
#include <core/util.h>

#include <KJob>

#include <QGuiApplication>
#include <QPalette>

using namespace GammaRay;

/*
 * TODO
 * - show job hierarchy
 * - show all job info messages
 * - show progress information
 * - allow to cancel/suspend if job supports that
 * - track runtime
 * - allow to clear the model
 * - tooltips with additional information (capabilities etc)
 */

KJobModel::KJobModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant KJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const KJobInfo &job = m_data.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return job.name;
        case 1:
            return job.type;
        case 2:
            return job.statusText;
        }
    } else if (role == Qt::ForegroundRole) {
        switch (job.state) {
        case KJobInfo::Finished:
        case KJobInfo::Deleted:
            return qApp->palette().brush(QPalette::Disabled, QPalette::Foreground);
        case KJobInfo::Error:
            return QVariant::fromValue<QColor>(Qt::red);
        case KJobInfo::Killed:
            return qApp->palette().link();
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int KJobModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int KJobModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

QVariant KJobModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Job");
        case 1:
            return tr("Type");
        case 2:
            return tr("Status");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

void KJobModel::objectAdded(QObject *obj)
{
    KJob *job = qobject_cast<KJob *>(obj);
    if (!job)
        return;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    KJobInfo jobInfo;
    jobInfo.job = job;
    connect(job, &KJob::result, this, &KJobModel::jobResult);
    connect(job, &KJob::finished, this, &KJobModel::jobFinished);
    connect(job, &KJob::infoMessage, this, &KJobModel::jobInfo);
    jobInfo.name = obj->objectName().isEmpty() ? Util::addressToString(obj) : obj->objectName();
    jobInfo.type = obj->metaObject()->className();
    jobInfo.state = KJobInfo::Running;
    m_data.push_back(jobInfo);
    endInsertRows();
}

void KJobModel::objectRemoved(QObject *obj)
{
    const int pos = indexOfJob(obj);
    if (pos < 0)
        return;

    // KJob dtor emits finished, so this shouldn't happen, in theory
    // We however seem to get here for very short-lived jobs that emit before objectAdded()
    // is called (while we wait for the vtable to be complete), so we only see the result
    // of their deleteLater().
    if (m_data.at(pos).state == KJobInfo::Running) {
        m_data[pos].state = KJobInfo::Deleted;
        m_data[pos].statusText = tr("Deleted");
        emit dataChanged(index(pos, 0), index(pos, columnCount() - 1));
    }
}

void KJobModel::jobResult(KJob *job)
{
    const int pos = indexOfJob(job);
    if (pos < 0)
        return;

    if (job->error()) {
        m_data[pos].state = KJobInfo::Error;
        m_data[pos].statusText = job->errorString();
    } else {
        if (m_data.at(pos).state == KJobInfo::Killed) {
            // we can get finished() before result(), which is perfectly fine
            m_data[pos].statusText.clear();
        }
        m_data[pos].state = KJobInfo::Finished;
    }

    emit dataChanged(index(pos, 0), index(pos, columnCount()-1));
}

void KJobModel::jobFinished(KJob *obj)
{
    const int pos = indexOfJob(obj);
    if (pos < 0)
        return;

    if (m_data.at(pos).state == KJobInfo::Running) {
        m_data[pos].state = KJobInfo::Killed;
        m_data[pos].statusText = tr("Killed");
    }

    emit dataChanged(index(pos, 0), index(pos, columnCount()-1));
}

void KJobModel::jobInfo(KJob *job, const QString &plainMessage)
{
    const int pos = indexOfJob(job);
    if (pos < 0)
        return;

    if (m_data.at(pos).state == KJobInfo::Running)
        m_data[pos].statusText = plainMessage;

    emit dataChanged(index(pos, 0), index(pos, columnCount()-1));
}

int KJobModel::indexOfJob(QObject *obj) const
{
    for (int i = 0; i < m_data.size(); ++i) {
        if (m_data.at(i).job == obj)
            return i;
    }
    return -1;
}
