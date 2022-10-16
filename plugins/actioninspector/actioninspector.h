/*
  actioninspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H

#include <core/toolfactory.h>

#include <QAction>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
class ActionInspector : public QObject
{
    Q_OBJECT

public:
    explicit ActionInspector(Probe *probe, QObject *parent = nullptr);
    ~ActionInspector() override;

public Q_SLOTS:
    static void triggerAction(int row);

private Q_SLOTS:
    void objectSelected(QObject *obj);

private:
    static void registerMetaTypes();
    QItemSelectionModel *m_selectionModel;
};

class ActionInspectorFactory : public QObject, public StandardToolFactory<QAction, ActionInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_actioninspector.json")

public:
    explicit ActionInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_ACTIONINSPECTOR_H
