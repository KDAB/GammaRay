/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWERINTERFACE_H
#define GAMMARAY_STATEMACHINEVIEWERINTERFACE_H

#include <QObject>
#include <QMetaType>
#include <QDataStream>

class QAbstractState;
class QAbstractTransition;

namespace GammaRay {

// note: typedef bring major pain, on the client side i.e. it would always look for
// signal/slots with the base type (or actually, the first type which was registered
// to the meta type system)...
struct TransitionId
{
  TransitionId(const TransitionId &id)
  : id(id.id)
  {}
  explicit TransitionId(QAbstractTransition *transition = 0)
  : id(reinterpret_cast<quint64>(transition))
  {}
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
  StateId(const StateId &id)
  : id(id.id)
  {}
  explicit StateId(QAbstractState *state= 0)
  : id(reinterpret_cast<quint64>(state))
  {}
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

enum StateType {
  OtherState,
  FinalState,
  HistoryState,
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

typedef QList<StateId> StateMachineConfiguration;

class StateMachineViewerInterface : public QObject
{
  Q_OBJECT
  public:
    explicit StateMachineViewerInterface(QObject *parent = 0);
    virtual ~StateMachineViewerInterface();

  public slots:
    virtual void toggleRunning() = 0;
    virtual void setMaximumDepth(int depth) = 0;

    virtual void repopulateGraph() = 0;

  signals:
    void statusChanged(bool haveStateMachine, bool running);
    void message(const QString &message);
    void aboutToRepopulateGraph();
    void graphRepopulated();
    void stateConfigurationChanged(const GammaRay::StateMachineConfiguration& config);
    void maximumDepthChanged(int depth);
    void transitionTriggered(GammaRay::TransitionId transition, const QString &label);
    void stateAdded(GammaRay::StateId state, GammaRay::StateId parent, bool hasChildren,
                    const QString &label, GammaRay::StateType type, bool connectToInitial);
    void stateEntered(GammaRay::StateId state);
    void stateExited(GammaRay::StateId state);
    void transitionAdded(GammaRay::TransitionId state, GammaRay::StateId source, GammaRay::StateId target,
                         const QString &label);
};

}

Q_DECLARE_METATYPE(GammaRay::StateId)
Q_DECLARE_METATYPE(GammaRay::TransitionId)
Q_DECLARE_METATYPE(GammaRay::StateMachineConfiguration)
Q_DECLARE_METATYPE(GammaRay::StateType)
Q_DECLARE_INTERFACE(GammaRay::StateMachineViewerInterface, "com.kdab.GammaRay.StateMachineViewer")

#endif // GAMMARAY_STATEMACHINEVIEWERINTERFACE_H
