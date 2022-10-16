/*
  eventmonitorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_EVENTMONITORCLIENT_H
#define GAMMARAY_EVENTMONITORCLIENT_H

#include "eventmonitorinterface.h"

namespace GammaRay {
class EventMonitorClient : public EventMonitorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::EventMonitorInterface)
public:
    explicit EventMonitorClient(QObject *parent = nullptr);
    ~EventMonitorClient() override;

public slots:
    virtual void clearHistory() override;
    virtual void recordAll() override;
    virtual void recordNone() override;
    virtual void showAll() override;
    virtual void showNone() override;
};
}

#endif // GAMMARAY_EVENTMONITORCLIENT_H
