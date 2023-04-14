/*
  scriptenginedebugger.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "scriptenginedebugger.h"

#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>

#include <QScriptEngine>
#include <QDebug>
#include <QtPlugin>

using namespace GammaRay;

/// NOTE: for crashes related to script engine debugger on shutdown, see:
///       https://bugreports.qt.nokia.com/browse/QTBUG-21548
///       Also it seems that we get another crash when the interrupt action
///       was triggered and we close the mainwindow.

ScriptEngineDebugger::ScriptEngineDebugger(Probe *probe, QObject *parent)
    : QObject(parent)
{
    auto *scriptEngineFilter = new ObjectTypeFilterProxyModel<QScriptEngine>(this);
    scriptEngineFilter->setSourceModel(probe->objectListModel());
    auto *singleColumnProxy = new SingleColumnObjectProxyModel(this);
    singleColumnProxy->setSourceModel(scriptEngineFilter);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ScriptEngines"), singleColumnProxy);
}

ScriptEngineDebugger::~ScriptEngineDebugger() = default;
