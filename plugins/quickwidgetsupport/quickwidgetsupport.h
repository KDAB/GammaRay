/*
  quickwidgetsupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKWIDGETSUPPRT_QUICKWIDGETSUPPORT_H
#define GAMMARAY_QUICKWIDGETSUPPRT_QUICKWIDGETSUPPORT_H

#include <core/toolfactory.h>

#include <QQuickWidget>

namespace GammaRay {
class QuickWidgetSupport : public QObject
{
    Q_OBJECT
public:
    explicit QuickWidgetSupport(Probe *probe, QObject *parent = nullptr);
    ~QuickWidgetSupport() override;

private slots:
    void objectAdded(QObject *obj);

private:
    Probe *m_probe;
};

class QuickWidgetSupportFactory : public QObject,
                                  public StandardToolFactory<QQuickWidget, QuickWidgetSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_quickwidgetsupport.json")

public:
    explicit QuickWidgetSupportFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
