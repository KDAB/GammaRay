/*
  statemachineviewerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERINTERFACE_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERINTERFACE_H

#include <QObject>
#include <QMetaType>
#include <QDataStream>
#include <QVector>

namespace GammaRay {
// note: typedef bring major pain, on the client side i.e. it would always look for
// signal/slots with the base type (or actually, the first type which was registered
// to the meta type system)...
struct TransitionId
{
    explicit TransitionId(quintptr transition = 0)
        : id(static_cast<quint64>(transition))
    {
    }
    operator quint64() const
    {
        return id;
    }
    quint64 id;
};

inline QDataStream &operator<<(QDataStream &out, TransitionId value)
{
    out << value.id;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, TransitionId &value)
{
    in >> value.id;
    return in;
}

struct StateId
{
    explicit StateId(quintptr state = 0)
        : id(static_cast<quint64>(state))
    {
    }
    operator quint64() const
    {
        return id;
    }
    quint64 id;
};

inline QDataStream &operator<<(QDataStream &out, StateId value)
{
    out << value.id;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, StateId &value)
{
    in >> value.id;
    return in;
}

enum StateType
{
    OtherState,
    FinalState,
    ShallowHistoryState,
    DeepHistoryState,
    StateMachineState
};

inline QDataStream &operator<<(QDataStream &out, StateType value)
{
    out << int(value);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, StateType &value)
{
    int val;
    in >> val;
    value = static_cast<StateType>(val);
    return in;
}

using StateMachineConfiguration = QVector<StateId>;

class StateMachineViewerInterface : public QObject
{
    Q_OBJECT
public:
    explicit StateMachineViewerInterface(QObject *parent = nullptr);
    ~StateMachineViewerInterface() override;

public slots:
    virtual void selectStateMachine(int index) = 0;
    virtual void toggleRunning() = 0;

    virtual void repopulateGraph() = 0;

signals:
    void statusChanged(bool haveStateMachine, bool running);
    void message(const QString &message);
    void aboutToRepopulateGraph();
    void graphRepopulated();
    void stateConfigurationChanged(const GammaRay::StateMachineConfiguration &config);
    void maximumDepthChanged(int depth);
    void transitionTriggered(GammaRay::TransitionId transition, const QString &label);
    void stateAdded(GammaRay::StateId state, GammaRay::StateId parent, bool hasChildren,
                    const QString &label, GammaRay::StateType type, bool connectToInitial);
    void stateEntered(GammaRay::StateId state);
    void stateExited(GammaRay::StateId state);
    void transitionAdded(GammaRay::TransitionId state, GammaRay::StateId source,
                         GammaRay::StateId target, const QString &label);
};
}

Q_DECLARE_METATYPE(GammaRay::StateId)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::StateId, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::TransitionId)
Q_DECLARE_METATYPE(GammaRay::StateMachineConfiguration)
Q_DECLARE_METATYPE(GammaRay::StateType)
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::StateMachineViewerInterface, "com.kdab.GammaRay.StateMachineViewer")
QT_END_NAMESPACE

#endif // GAMMARAY_STATEMACHINEVIEWERINTERFACE_H
