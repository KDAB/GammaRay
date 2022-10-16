/*
  processtracker.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "processtracker.h"

#include <QDebug>
#include <QTimer>

using namespace GammaRay;

class ProcessTracker::D : public QObject
{
    Q_OBJECT

public:
    GammaRay::ProcessTracker *tracker;
    GammaRay::ProcessTrackerBackend *backend;
    QTimer *ticker;
    GammaRay::ProcessTrackerInfo previousInfo;
    qint64 pid;

    explicit D(GammaRay::ProcessTracker *tracker)
        : QObject(tracker)
        , tracker(tracker)
        , backend(nullptr)
        , ticker(new QTimer(this))
        , pid(-1)
    {
        ticker->setSingleShot(false);

        connect(ticker, &QTimer::timeout, this, &D::requestUpdate);
    }

public slots:
    void requestUpdate() const
    {
        if (!backend) {
            qWarning("%s: Backend not set", Q_FUNC_INFO);
            return;
        }

        if (pid < 0) {
            qWarning("%s: Pid not set", Q_FUNC_INFO);
            return;
        }

        backend->checkProcess(pid);
    }

    void processChecked(const GammaRay::ProcessTrackerInfo &info)
    {
        if (pid != info.pid) {
            return;
        }

        if (info != previousInfo) {
            previousInfo = info;
            emit tracker->infoChanged(info);
        }
    }
};

ProcessTracker::ProcessTracker(QObject *parent)
    : QObject(parent)
    , d(new D(this))
{
    qRegisterMetaType<GammaRay::ProcessTrackerInfo>("GammaRay::ProcessTrackerInfo");
}

ProcessTracker::~ProcessTracker()
{
    stop();
}

GammaRay::ProcessTrackerBackend *ProcessTracker::backend() const
{
    return d->backend;
}

void ProcessTracker::setBackend(GammaRay::ProcessTrackerBackend *backend)
{
    if (backend == d->backend) {
        return;
    }

    if (d->backend) {
        disconnect(d->backend, &ProcessTrackerBackend::processChecked,
                   d.data(), &D::processChecked);
    }

    d->backend = backend;

    if (d->backend) {
        connect(d->backend, &ProcessTrackerBackend::processChecked,
                d.data(), &D::processChecked, Qt::QueuedConnection);
    }

    emit backendChanged(d->backend);
}

qint64 ProcessTracker::pid() const
{
    return d->pid;
}

bool ProcessTracker::isActive() const
{
    return d->ticker->isActive();
}

void ProcessTracker::setPid(qint64 pid)
{
    d->previousInfo = ProcessTrackerInfo();
    d->pid = pid;
}

void ProcessTracker::start(int msecs)
{
    d->previousInfo = ProcessTrackerInfo();
    d->ticker->start(msecs);
}

void ProcessTracker::stop()
{
    d->previousInfo = ProcessTrackerInfo();
    d->ticker->stop();
}

bool ProcessTrackerInfo::operator==(const GammaRay::ProcessTrackerInfo &other) const
{
    return pid == other.pid && traced == other.traced && state == other.state;
}

bool ProcessTrackerInfo::operator!=(const GammaRay::ProcessTrackerInfo &other) const
{
    return !operator==(other);
}

ProcessTrackerBackend::ProcessTrackerBackend(QObject *parent)
    : QObject(parent)
{
}

#include "processtracker.moc"
