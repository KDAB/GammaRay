#ifndef STATEMACHINEVIEWERWIDGETNG_H
#define STATEMACHINEVIEWERWIDGETNG_H

#include <ui/tooluifactory.h>
#include "statemachineviewerinterface.h"

#include <QWidget>

namespace KDSME {
class State;
class StateMachine;
class StateMachineView;
class Transition;
class View;
}

namespace GammaRay {
namespace Ui {
class StateMachineViewer;
}

class StateMachineViewerWidgetNG : public QWidget
{
  Q_OBJECT

public:
  explicit StateMachineViewerWidgetNG(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~StateMachineViewerWidgetNG();

private slots:
  void showMessage(const QString &message);
  void stateAdded(const GammaRay::StateId stateId, const GammaRay::StateId parentId, const bool hasChildren,
                  const QString& label, const GammaRay::StateType type, const bool connectToInitial);
  void stateConfigurationChanged(const GammaRay::StateMachineConfiguration &config);
  void transitionAdded(const GammaRay::TransitionId transitionId, const GammaRay::StateId source,
                       const GammaRay::StateId target, const QString& label);
  void statusChanged(const bool haveStateMachine, const bool running);
  void transitionTriggered(GammaRay::TransitionId transition, const QString &label);
  void stateModelReset();

  void repopulateView();
  void clearGraph();

  void setMaximumMegaPixels(int);

private:
  int maximumMegaPixels() const;

  QScopedPointer<Ui::StateMachineViewer> m_ui;

  KDSME::StateMachineView* m_stateMachineView;
  KDSME::View* m_currentView;
  StateMachineViewerInterface *m_interface;

  QHash<StateId, KDSME::State*> m_idToStateMap;
  QHash<TransitionId, KDSME::Transition*> m_idToTransitionMap;
  KDSME::StateMachine* m_machine;
};

class StateMachineViewerUiFactory : public QObject, public StandardToolUiFactory<StateMachineViewerWidgetNG>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.StateMachineViewerNGUi")
};

}

#endif // STATEMACHINEVIEWERWIDGETNG_H
