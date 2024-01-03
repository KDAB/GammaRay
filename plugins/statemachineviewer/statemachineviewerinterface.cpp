/*
  statemachineviewerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "statemachineviewerinterface.h"
#include <common/objectbroker.h>
#include <common/streamoperators.h>

using namespace GammaRay;

StateMachineViewerInterface::StateMachineViewerInterface(QObject *parent)
    : QObject(parent)
{
    StreamOperators::registerOperators<StateId>();
    StreamOperators::registerOperators<TransitionId>();
    StreamOperators::registerOperators<StateMachineConfiguration>();
    StreamOperators::registerOperators<StateType>();
    ObjectBroker::registerObject<StateMachineViewerInterface *>(this);
}

StateMachineViewerInterface::~StateMachineViewerInterface() = default;
