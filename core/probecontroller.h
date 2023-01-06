/*
  probecontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
