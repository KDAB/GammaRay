/*
  methodsextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_METHODSEXTENSION_H
#define GAMMARAY_OBJECTINSPECTOR_METHODSEXTENSION_H

#include "common/tools/objectinspector/methodsextensioninterface.h"
#include "core/propertycontrollerextension.h"

#include <QPointer>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;
class ObjectMethodModel;
class MethodArgumentModel;
class MultiSignalMapper;

class MethodsExtension : public MethodsExtensionInterface, public PropertyControllerExtension
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MethodsExtensionInterface)

public:
    explicit MethodsExtension(PropertyController *controller);
    ~MethodsExtension() override;

    bool setQObject(QObject *object) override;
    bool setMetaObject(const QMetaObject *metaObject) override;

public slots:
    void activateMethod() override;
    void invokeMethod(Qt::ConnectionType connectionType) override;
    void connectToSignal() override;

private slots:
    void signalEmitted(QObject *sender, int signalIndex, const QVector<QVariant> &args);

private:
    ObjectMethodModel *m_model;
    QStandardItemModel *m_methodLogModel;
    MethodArgumentModel *m_methodArgumentModel;
    MultiSignalMapper *m_signalMapper;
    QPointer<QObject> m_object;
};
}

#endif // METHODSEXTENSION_H
