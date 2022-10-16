/*
  timertopclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TIMERTOP_TIMERTOPCLIENT_H
#define GAMMARAY_TIMERTOP_TIMERTOPCLIENT_H

#include "timertopinterface.h"

namespace GammaRay {
class TimerTopClient : public TimerTopInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::TimerTopInterface)

public:
    explicit TimerTopClient(QObject *parent = nullptr);
    ~TimerTopClient() override;

public slots:
    void clearHistory() override;
};
}

#endif // GAMMARAY_TIMERTOP_TIMERTOPCLIENT_H
