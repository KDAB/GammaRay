/*
  signalmonitor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#ifndef GAMMARAY_SIGNALMONITOR_H
#define GAMMARAY_SIGNALMONITOR_H

#include "signalmonitorinterface.h"

#include <core/toolfactory.h>

class QTimer;

namespace GammaRay {

class SignalMonitor : public SignalMonitorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::SignalMonitorInterface)
  public:
    explicit SignalMonitor(ProbeInterface *probe, QObject *parent = 0);
    ~SignalMonitor();

  public slots:
    void sendClockUpdates(bool enabled) /*Q_DECL_OVERRIDE*/;

  private slots:
    void timeout();

  private:
    QTimer *m_clock;

};

class SignalMonitorFactory : public QObject, public StandardToolFactory<QObject, SignalMonitor>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.SignalMonitor")
  public:
    explicit SignalMonitorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Signals");
    }
};

} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITOR_H
