/*
  scriptenginedebuggerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SCRIPTENGINEDEBUGGER_SCRIPTENGINEDEBUGGERWIDGET_H
#define GAMMARAY_SCRIPTENGINEDEBUGGER_SCRIPTENGINEDEBUGGERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include <QWidget>
#include <QScriptEngine>

QT_BEGIN_NAMESPACE
class QScriptEngineDebugger;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class ScriptEngineDebuggerWidget;
}

class ScriptEngineDebuggerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEngineDebuggerWidget(QWidget *parent = nullptr);
    ~ScriptEngineDebuggerWidget() override;
private slots:
    void scriptEngineSelected(int index);

private:
    QScopedPointer<Ui::ScriptEngineDebuggerWidget> ui;
    UIStateManager m_stateManager;
    QScriptEngineDebugger *debugger;
};

class ScriptEngineDebuggerUiFactory : public QObject,
                                      public StandardToolUiFactory<ScriptEngineDebuggerWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(
        IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_scriptenginedebugger.json")
};
}

#endif // GAMMARAY_SCRIPTENGINEDEBUGGER_H
