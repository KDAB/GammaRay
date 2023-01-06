/*
  statemachineviewerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
