/*
  scriptenginedebuggerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

/// NOTE: for crashes related to script engine debugger on shutdown, see:
///       https://bugreports.qt.nokia.com/browse/QTBUG-21548
///       Also it seems that we get another crash when the interrupt action
///       was triggered and we close the mainwindow.

ScriptEngineDebuggerWidget::ScriptEngineDebuggerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScriptEngineDebuggerWidget)
    , m_stateManager(this)
    , debugger(new QScriptEngineDebugger(this))
{
    ui->setupUi(this);
    ui->scriptEngineComboBox->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.ScriptEngines")));
    connect(ui->scriptEngineComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &ScriptEngineDebuggerWidget::scriptEngineSelected);

    ui->verticalLayout_10->addWidget(debugger->standardWindow());

    if (ui->scriptEngineComboBox->count())
        scriptEngineSelected(0);
}

ScriptEngineDebuggerWidget::~ScriptEngineDebuggerWidget()
{
    debugger->detach();
}

void ScriptEngineDebuggerWidget::scriptEngineSelected(int index)
{
    QObject *obj = ui->scriptEngineComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject *>();
    QScriptEngine *engine = qobject_cast<QScriptEngine *>(obj);
    if (engine) {
        debugger->attachTo(engine);
        // FIXME: if we'd do that, we'd get crashes on shutdown.
        // debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    }
}
