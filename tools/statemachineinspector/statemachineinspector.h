#ifndef ENDOSCOPE_STATEMACHINEINSPECTOR_H
#define ENDOSCOPE_STATEMACHINEINSPECTOR_H

#include <qwidget.h>
#include <toolfactory.h>

#include <QtCore/QStateMachine>

class QItemSelection;

namespace Endoscope {

class StateModel;
class TransitionModel;
namespace Ui { class StateMachineInspector; }

class StateMachineInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit StateMachineInspector( ProbeInterface *probe, QWidget *parent = 0 );

  private slots:
    void stateMachineSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void stateSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::StateMachineInspector> ui;
    StateModel *m_stateModel;
    TransitionModel *m_transitionModel;
};

class StateMachineInspectorFactory : public QObject, public StandardToolFactory<QStateMachine, StateMachineInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("State Machines"); }
};

}

#endif // ENDOSCOPE_STATEMACHINEINSPECTOR_H
