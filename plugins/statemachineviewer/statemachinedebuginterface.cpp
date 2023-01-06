/*
  statemachinedebuginterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Arne Petersen <jan.petersen@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "statemachinedebuginterface.h"


using namespace GammaRay;

namespace GammaRay {

StateMachineDebugInterface::StateMachineDebugInterface(QObject *parent)
    : QObject(parent)
{
}

StateMachineDebugInterface::~StateMachineDebugInterface() = default;

bool StateMachineDebugInterface::isDescendantOf(State ascendant, State state) const
{
    if (state == rootState())
        return false;

    State parent = parentState(state);
    if (parent == ascendant)
        return true;
    return isDescendantOf(ascendant, parent);
}

}
