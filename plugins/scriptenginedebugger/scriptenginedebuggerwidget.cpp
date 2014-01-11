/*
  scriptenginedebuggerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "scriptenginedebuggerwidget.h"
#include "ui_scriptenginedebuggerwidget.h"

#include <common/objectmodel.h>
#include <common/objectbroker.h>

#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QDebug>
#include <QMainWindow>
#include <QtPlugin>

using namespace GammaRay;

///NOTE: for crashes related to script engine debugger on shutdown, see:
///      https://bugreports.qt.nokia.com/browse/QTBUG-21548
///      Also it seems that we get another crash when the interrupt action
///      was triggered and we close the mainwindow.

ScriptEngineDebuggerWidget::ScriptEngineDebuggerWidget(QWidget *parent)
  : QWidget(parent), ui(new Ui::ScriptEngineDebuggerWidget), debugger(new QScriptEngineDebugger(this))
{
  ui->setupUi(this);
  ui->scriptEngineComboBox->setModel(ObjectBroker::model("com.kdab.GammaRay.ScriptEngines"));
  connect(ui->scriptEngineComboBox, SIGNAL(activated(int)), SLOT(scriptEngineSelected(int)));

  ui->verticalLayout_10->addWidget(debugger->standardWindow());

  if (ui->scriptEngineComboBox->count()) {
    scriptEngineSelected(0);
  }
}

ScriptEngineDebuggerWidget::~ScriptEngineDebuggerWidget()
{
  debugger->detach();
}

void ScriptEngineDebuggerWidget::scriptEngineSelected(int index)
{
  QObject *obj =
    ui->scriptEngineComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QScriptEngine *engine = qobject_cast<QScriptEngine*>(obj);
  if (engine) {
    debugger->attachTo(engine);
// FIXME: if we'd do that, we'd get crashes on shutdown.
//     debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
  }
}
