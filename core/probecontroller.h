/*
  probecontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROBECONTROLLER_H
#define GAMMARAY_PROBECONTROLLER_H

#include <common/probecontrollerinterface.h>

namespace GammaRay {
/** @brief Server-side part for the object property inspector.
 *  Use this to integrate a property inspector like in the QObject view into your tool.
 */
class ProbeController : public ProbeControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ProbeControllerInterface)
public:
    explicit ProbeController(QObject *parent = nullptr);

public slots:
    void detachProbe() override;
    void quitHost() override;
};
}

#endif // GAMMARAY_PROBECONTROLLER_H
