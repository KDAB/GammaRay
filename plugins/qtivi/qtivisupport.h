/*
  qtivisupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QTIVISUPPORT_H
#define GAMMARAY_QTIVISUPPORT_H

#include <core/toolfactory.h>

#include <QIviServiceObject>

namespace GammaRay {

class QtIviSupport : public QObject
{
    Q_OBJECT
public:
    explicit QtIviSupport(Probe *probe, QObject *parent = nullptr);
};

class QtIviSupportFactory : public QObject, public StandardToolFactory<QIviServiceObject, QtIviSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_qtivi.json")

public:
    explicit QtIviSupportFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};

}

#endif
