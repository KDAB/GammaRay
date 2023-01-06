/*
  statemachinedebuginterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Arne Petersen <jan.petersen@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEDEBUGINTERFACE_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEDEBUGINTERFACE_H

#include <QObject>

#include "statemachineviewerinterface.h"

namespace GammaRay {

struct State
{
    explicit State(quintptr id = 0)
        : m_id(id)
    {
    }

    operator quintptr() const
    {
        return m_id;
    }

    quintptr m_id;
};

struct Transition
{
    explicit Transition(quintptr id = 0)
        : m_id(id)
    {
    }

    operator quintptr() const
    {
        return m_id;
    }

    quintptr m_id;
};

class StateMachineDebugInterface : public QObject
{
    Q_OBJECT

public:
    explicit StateMachineDebugInterface(QObject *parent = nullptr);
    ~StateMachineDebugInterface() override;

    virtual bool isRunning() const = 0;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual QVector<State> configuration() const = 0;

    virtual State rootState() const = 0;
    virtual bool stateValid(State state) const = 0;
    virtual QVector<State> stateChildren(State state) const = 0;
    virtual State parentState(State state) const = 0;

    virtual bool isInitialState(State state) const = 0;
    virtual QString transitions(State state) const = 0;

    virtual QString stateLabel(State state) const = 0;
    virtual QString stateDisplay(State state) const = 0;
    virtual QString stateDisplayType(State state) const = 0;
    virtual StateType stateType(State state) const = 0;
    virtual QVector<Transition> stateTransitions(State state) const = 0;
    virtual QObject *stateObject(State state) const = 0;

    virtual QString transitionLabel(Transition transition) const = 0;
    virtual State transitionSource(Transition transition) const = 0;
    virtual QVector<State> transitionTargets(Transition transition) const = 0;

    bool isDescendantOf(State ascendant, State state) const;

signals:
    void runningChanged(bool running);

    void stateEntered(State state);
    void stateExited(State state);

    void transitionTriggered(Transition transition);

    void logMessage(const QString &label, const QString &message);
};
}

Q_DECLARE_METATYPE(GammaRay::State)
Q_DECLARE_METATYPE(GammaRay::Transition)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::State, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(GammaRay::Transition, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

#endif
