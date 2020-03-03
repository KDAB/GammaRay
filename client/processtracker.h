/*
  processtracker.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    enum State {
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
