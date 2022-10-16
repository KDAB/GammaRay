/*
  probecontrollerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBECONTROLLERINTERFACE_H
#define GAMMARAY_PROBECONTROLLERINTERFACE_H

#include <QObject>
#include <QDataStream>
#include <QDebug>
#include <QMetaType>
#include <QVector>

namespace GammaRay {
/*! Probe and host process remote control functions. */
class ProbeControllerInterface : public QObject
{
    Q_OBJECT

public:
    explicit ProbeControllerInterface(QObject *parent = nullptr);
    ~ProbeControllerInterface() override;

    /*! Terminate host application. */
    virtual void quitHost() = 0;

    /*! Detach GammaRay but keep host application running. */
    virtual void detachProbe() = 0;

private:
    Q_DISABLE_COPY(ProbeControllerInterface)
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ProbeControllerInterface,
                    "com.kdab.GammaRay.ProbeControllerInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_PROBECONTROLLERINTERFACE_H
