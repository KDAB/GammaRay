/*
  processtracker.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROCESSTRACKER_H
#define GAMMARAY_PROCESSTRACKER_H

#include "gammaray_client_export.h"

#include <QObject>
#include <QSharedPointer>
#include <QMetaType>

namespace GammaRay {
class ProcessTrackerBackend;
class ProcessTrackerInfo;

class GAMMARAY_CLIENT_EXPORT ProcessTracker : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        Unknown = -1,
        Running,
        Suspended
    };

    explicit ProcessTracker(QObject *parent = nullptr);
    ~ProcessTracker();

    GammaRay::ProcessTrackerBackend *backend() const;
    void setBackend(GammaRay::ProcessTrackerBackend *backend);

    qint64 pid() const;
    bool isActive() const;

public slots:
    void setPid(qint64 pid);
    void start(int msecs = 3000);
    void stop();

signals:
    void backendChanged(GammaRay::ProcessTrackerBackend *backend);
    void infoChanged(const GammaRay::ProcessTrackerInfo &info);

private:
    class D;
    QSharedPointer<D> d;
};

class GAMMARAY_CLIENT_EXPORT ProcessTrackerInfo
{
public:
    explicit ProcessTrackerInfo(qint64 pid = -1, bool traced = false,
                                GammaRay::ProcessTracker::State state = GammaRay::ProcessTracker::Unknown)
        : pid(pid)
        , traced(traced)
        , state(state)
    {
    }

    qint64 pid;
    bool traced;
    GammaRay::ProcessTracker::State state;

    bool operator==(const GammaRay::ProcessTrackerInfo &other) const;
    bool operator!=(const GammaRay::ProcessTrackerInfo &other) const;
};

class GAMMARAY_CLIENT_EXPORT ProcessTrackerBackend : public QObject
{
    Q_OBJECT

public:
    explicit ProcessTrackerBackend(QObject *parent = nullptr);

public slots:
    virtual void checkProcess(qint64 pid) = 0;

signals:
    void processChecked(const GammaRay::ProcessTrackerInfo &info);
};

}

Q_DECLARE_METATYPE(GammaRay::ProcessTrackerInfo)

#endif // GAMMARAY_PROCESSTRACKER_H
