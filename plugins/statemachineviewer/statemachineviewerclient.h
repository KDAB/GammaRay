/*
  statemachineviewerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERCLIENT_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERCLIENT_H

#include "statemachineviewerinterface.h"

namespace GammaRay {
class StateMachineViewerClient : public StateMachineViewerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::StateMachineViewerInterface)
public:
    explicit StateMachineViewerClient(QObject *parent = nullptr);

    void selectStateMachine(int index) override;
    void toggleRunning() override;
    void repopulateGraph() override;
};
}

#endif // GAMMARAY_STATEMACHINEVIEWERCLIENT_H
