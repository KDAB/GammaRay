/*
  webinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WEBINSPECTOR_WEBINSPECTOR_H
#define GAMMARAY_WEBINSPECTOR_WEBINSPECTOR_H

#include <core/toolfactory.h>

namespace GammaRay {
class WebInspector : public QObject
{
    Q_OBJECT
public:
    explicit WebInspector(Probe *probe, QObject *parent = nullptr);

private slots:
    static void objectAdded(QObject *obj);
};

class WebInspectorFactory : public QObject, public ToolFactory
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_webinspector.json")

public:
    explicit WebInspectorFactory(QObject *parent = nullptr);
    QString id() const override;
    void init(Probe *probe) override;
};
}

#endif // GAMMARAY_WEBINSPECTOR_H
