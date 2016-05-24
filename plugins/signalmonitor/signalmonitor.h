/*
  signalmonitor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#ifndef GAMMARAY_SIGNALMONITOR_H
#define GAMMARAY_SIGNALMONITOR_H

#include "signalmonitorinterface.h"

#include <core/toolfactory.h>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {

class SignalMonitor : public SignalMonitorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::SignalMonitorInterface)
  public:
    explicit SignalMonitor(ProbeInterface *probe, QObject *parent = 0);
    ~SignalMonitor();

  public slots:
    void sendClockUpdates(bool enabled) Q_DECL_OVERRIDE;

  private slots:
    void timeout();

  private:
    QTimer *m_clock;

};

class SignalMonitorFactory : public QObject, public StandardToolFactory<QObject, SignalMonitor>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_signalmonitor.json")
  public:
    explicit SignalMonitorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    QString name() const Q_DECL_OVERRIDE;
};

} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITOR_H
