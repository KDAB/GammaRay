/*
  qtivisupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
