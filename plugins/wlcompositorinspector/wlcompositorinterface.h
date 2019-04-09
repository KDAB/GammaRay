/*
  wlcompositorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#ifndef WLCOMPOSITORINTERFACE_H
#define WLCOMPOSITORINTERFACE_H

#include <config-gammaray.h>

#include <QObject>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

namespace GammaRay {

class WlCompositorInterface : public QObject
{
  Q_OBJECT
public:
  explicit WlCompositorInterface(QObject *parent);
  ~WlCompositorInterface() override;

public slots:
  virtual void connected() = 0;
  virtual void disconnected() = 0;
  virtual void setSelectedClient(int index) = 0;
  virtual void setSelectedResource(uint id) = 0;

signals:
  void logMessage(quint64 pid, qint64 time, const QByteArray &msg);
  void setLoggingClient(quint64 pid);
  void resetLog();

};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::WlCompositorInterface, "com.kdab.GammaRay.WlCompositor")
QT_END_NAMESPACE

#endif
