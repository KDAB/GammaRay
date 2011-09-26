/*
  statemachineinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_STATEMACHINEINSPECTOR_H
#define GAMMARAY_STATEMACHINEINSPECTOR_H

#include <qwidget.h>
#include <toolfactory.h>

#include <QtCore/QStateMachine>

class QItemSelection;

namespace GammaRay {

class StateModel;
class TransitionModel;
namespace Ui { class StateMachineInspector; }

class StateMachineInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit StateMachineInspector(ProbeInterface *probe, QWidget *parent = 0);

  private slots:
    void stateMachineSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void stateSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::StateMachineInspector> ui;
    StateModel *m_stateModel;
    TransitionModel *m_transitionModel;
};

class StateMachineInspectorFactory :
    public QObject, public StandardToolFactory<QStateMachine, StateMachineInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit StateMachineInspectorFactory(QObject *parent) : QObject(parent) {}
    inline QString name() const { return tr("State Machines"); }
};

}

#endif // GAMMARAY_STATEMACHINEINSPECTOR_H
