/*
  scriptenginedebugger.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCRIPTENGINEDEBUGGER_SCRIPTENGINEDEBUGGER_H
#define GAMMARAY_SCRIPTENGINEDEBUGGER_SCRIPTENGINEDEBUGGER_H

#include <core/toolfactory.h>

#include <QScriptEngine>

namespace GammaRay {
class ScriptEngineDebugger : public QObject
{
    Q_OBJECT
public:
    explicit ScriptEngineDebugger(Probe *probe, QObject *parent = nullptr);
    ~ScriptEngineDebugger() override;
};

class ScriptEngineDebuggerFactory : public QObject,
                                    public StandardToolFactory<QScriptEngine, ScriptEngineDebugger>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_scriptenginedebugger.json")

public:
    explicit ScriptEngineDebuggerFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_SCRIPTENGINEDEBUGGER_H
