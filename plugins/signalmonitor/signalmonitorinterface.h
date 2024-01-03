/*
  signalmonitorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALMONITORINTERFACE_H
#define GAMMARAY_SIGNALMONITORINTERFACE_H

#include <QObject>

namespace GammaRay {
class SignalMonitorInterface : public QObject
{
    Q_OBJECT
public:
    explicit SignalMonitorInterface(QObject *parent = nullptr);
    ~SignalMonitorInterface() override;

public slots:
    virtual void sendClockUpdates(bool enabled) = 0;

signals:
    void clock(qlonglong msecs);
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::SignalMonitorInterface,
                    "com.kdab.GammaRay.SignalMonitorInterface/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_SIGNALMONITORINTERFACE_H
