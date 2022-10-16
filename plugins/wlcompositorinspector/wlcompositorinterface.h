/*
  wlcompositorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
