/*
  probecontrollerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
