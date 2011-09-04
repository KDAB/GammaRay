/*
  scriptenginedebugger.cpp

  This file is part of Endoscope, the Qt application inspection and
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
