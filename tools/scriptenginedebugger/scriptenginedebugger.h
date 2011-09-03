#ifndef ENDOSCOPE_SCRIPTENGINEDEBUGGER_H
#define ENDOSCOPE_SCRIPTENGINEDEBUGGER_H

#include <toolfactory.h>
#include <qwidget.h>
#include <qscriptengine.h>

namespace Endoscope {

namespace Ui { class ScriptEngineDebugger; }

class ScriptEngineDebugger : public QWidget
{
  Q_OBJECT
  public:
    explicit ScriptEngineDebugger(ProbeInterface* probe, QWidget* parent = 0);

  private slots:
    void scriptEngineSelected(int index);

  private:
    QScopedPointer<Ui::ScriptEngineDebugger> ui;
};

class ScriptEngineDebuggerFactory : public QObject, public StandardToolFactory<QScriptEngine, ScriptEngineDebugger>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Script Engines"); }
};

}

#endif // ENDOSCOPE_SCRIPTENGINEDEBUGGER_H
