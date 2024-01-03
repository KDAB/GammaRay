/*
  qmlsupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLSUPPORT_QMLSUPPORT_H
#define GAMMARAY_QMLSUPPORT_QMLSUPPORT_H

#include <core/toolfactory.h>

#include <QJSEngine>

namespace GammaRay {
class QmlSupport : public QObject
{
    Q_OBJECT
public:
    explicit QmlSupport(Probe *probe, QObject *parent = nullptr);
};

class QmlSupportFactory : public QObject, public StandardToolFactory<QJSEngine, QmlSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_qmlsupport.json")

public:
    explicit QmlSupportFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
