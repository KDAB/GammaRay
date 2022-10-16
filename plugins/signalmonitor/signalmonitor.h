/*
  signalmonitor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SIGNALMONITOR_H
#define GAMMARAY_SIGNALMONITOR_H

#include "signalmonitorinterface.h"

#include <core/toolfactory.h>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelectionModel;
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class SignalMonitor : public SignalMonitorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::SignalMonitorInterface)
public:
    explicit SignalMonitor(Probe *probe, QObject *parent = nullptr);
    ~SignalMonitor() override;

public slots:
    void sendClockUpdates(bool enabled) override;

private slots:
    void timeout();
    void objectSelected(QObject *obj);

private:
    QTimer *m_clock;
    QAbstractItemModel *m_objModel;
    QItemSelectionModel *m_objSelectionModel;
};

class SignalMonitorFactory : public QObject, public StandardToolFactory<QObject, SignalMonitor>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_signalmonitor.json")
public:
    explicit SignalMonitorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITOR_H
