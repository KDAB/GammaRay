#include "scriptenginedebugger.h"
#include "ui_scriptenginedebugger.h"

#include <probeinterface.h>
#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>

#include <QtScript/qscriptengine.h>
#include <QtScriptTools/QScriptEngineDebugger>
#include <QDebug>
#include <QMainWindow>

using namespace Endoscope;

ScriptEngineDebugger::ScriptEngineDebugger(ProbeInterface* probe, QWidget* parent):
  QWidget( parent ),
  ui( new Ui::ScriptEngineDebugger )
{
  ui->setupUi( this );

  ObjectTypeFilterProxyModel<QScriptEngine> *scriptEngineFilter = new ObjectTypeFilterProxyModel<QScriptEngine>( this );
  scriptEngineFilter->setSourceModel( probe->objectListModel() );
  SingleColumnObjectProxyModel* singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( scriptEngineFilter );
  ui->scriptEngineComboBox->setModel( singleColumnProxy );
  connect( ui->scriptEngineComboBox, SIGNAL(activated(int)), SLOT(scriptEngineSelected(int)) );
}

ScriptEngineDebugger::~ScriptEngineDebugger()
{
  delete ui;
}

void ScriptEngineDebugger::scriptEngineSelected(int index)
{
  QObject* obj = ui->scriptEngineComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QScriptEngine *engine = qobject_cast<QScriptEngine*>( obj );
  if ( engine ) {
    QScriptEngineDebugger *debugger = new QScriptEngineDebugger( this );
    qDebug() << "Attaching debugger" << engine;
    debugger->attachTo( engine );
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    debugger->standardWindow()->show();
  }
}


#include "scriptenginedebugger.moc"
