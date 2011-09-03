#ifndef ENDOSCOPE_SCRIPTENGINEDEBUGGER_H
#define ENDOSCOPE_SCRIPTENGINEDEBUGGER_H

#include <toolinterface.h>
#include <qwidget.h>
#include <qscriptengine.h>

namespace Endoscope {

namespace Ui { class ScriptEngineDebugger; }

class ScriptEngineDebugger : public QWidget
{
  Q_OBJECT
  public:
    explicit ScriptEngineDebugger(ProbeInterface* probe, QWidget* parent = 0);
    virtual ~ScriptEngineDebugger();

  private slots:
    void scriptEngineSelected(int index);

  private:
    Ui::ScriptEngineDebugger* ui;
};

class ScriptEngineDebuggerInterface : public QObject, public ToolInterface
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolInterface)
  public:
    inline QString name() const { return tr("Script Engines"); }
    inline QStringList supportedTypes() const { return QStringList( QScriptEngine::staticMetaObject.className() ); }
    inline QWidget* createInstance( ProbeInterface *probeIface, QWidget *parent ) { return new ScriptEngineDebugger( probeIface, parent ); }
};

}

#endif // ENDOSCOPE_SCRIPTENGINEDEBUGGER_H
