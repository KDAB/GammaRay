/*
  timertopclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
