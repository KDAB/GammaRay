/*
  objectinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTORWIDGET_H
#define GAMMARAY_OBJECTINSPECTOR_OBJECTINSPECTORWIDGET_H

#include <QWidget>
#include "tooluifactory.h"
#include "propertywidget.h"
#include "propertiestab.h"
#include "methodstab.h"
#include "connectionstab.h"
#include "enumstab.h"
#include "classinfotab.h"
#include "propertiesextensionclient.h"
#include "methodsextensionclient.h"
#include "connectionsextensionclient.h"
#include "applicationattributetab.h"
#include "bindingtab.h"
#include "stacktracetab.h"

#include <common/objectbroker.h>

#include <ui/uistatemanager.h>
#include <ui/deferredtreeview.h>
#include <ui/favoritesitemview.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class ObjectInspectorWidget;
}

template<typename T>
static QObject *createExtension(const QString &name, QObject *parent)
{
    return new T(name, parent);
}

class ObjectInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectInspectorWidget(QWidget *parent = nullptr);
    ~ObjectInspectorWidget() override;

private slots:
    void objectSelectionChanged(const QItemSelection &selection);
    void objectContextMenuRequested(const QPoint &pos);
    void propertyWidgetTabsChanged();

private:
    QScopedPointer<Ui::ObjectInspectorWidget> ui;
    UIStateManager m_stateManager;
};

class ObjectInspectorFactory : public QObject, public ToolUiFactory
{
    Q_OBJECT
public:
    QString id() const override
    {
        return QStringLiteral("GammaRay::ObjectInspector");
    }
    QString name() const override
    {
        return tr("Objects");
    }
    QWidget *createWidget(QWidget *parentWidget) override
    {
        return new ObjectInspectorWidget(
            parentWidget);
    }
    void initUi() override
    {
        PropertyWidget::registerTab<PropertiesTab>(QStringLiteral("properties"), tr("Properties"),
                                                   PropertyWidgetTabPriority::First);
        ObjectBroker::registerClientObjectFactoryCallback<PropertiesExtensionInterface *>(
            createExtension<PropertiesExtensionClient>);
        PropertyWidget::registerTab<MethodsTab>(QStringLiteral("methods"), tr("Methods"),
                                                PropertyWidgetTabPriority::Basic - 1);
        ObjectBroker::registerClientObjectFactoryCallback<MethodsExtensionInterface *>(
            createExtension<MethodsExtensionClient>);
        PropertyWidget::registerTab<ConnectionsTab>(QStringLiteral("connections"), tr("Connections"),
                                                    PropertyWidgetTabPriority::Basic - 1);
        ObjectBroker::registerClientObjectFactoryCallback<ConnectionsExtensionInterface *>(
            createExtension<ConnectionsExtensionClient>);
        PropertyWidget::registerTab<EnumsTab>(QStringLiteral("enums"), tr("Enums"), PropertyWidgetTabPriority::Exotic - 1);
        PropertyWidget::registerTab<ClassInfoTab>(QStringLiteral("classInfo"), tr("Class Info"),
                                                  PropertyWidgetTabPriority::Exotic - 1);
        PropertyWidget::registerTab<ApplicationAttributeTab>(QStringLiteral("applicationAttributes"),
                                                             tr("Attributes"),
                                                             PropertyWidgetTabPriority::Advanced);
        PropertyWidget::registerTab<BindingTab>(QStringLiteral("bindings"), tr("Bindings"),
                                                PropertyWidgetTabPriority::Advanced);
        PropertyWidget::registerTab<StackTraceTab>(QStringLiteral("stackTrace"), tr("Stack Trace"),
                                                   PropertyWidgetTabPriority::Exotic);
    }
};
}

#endif // GAMMARAY_OBJECTINSPECTOR_H
