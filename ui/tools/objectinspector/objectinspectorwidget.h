/*
  objectinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#include <common/objectbroker.h>

#include <ui/uistatemanager.h>

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
    explicit ObjectInspectorWidget(QWidget *parent = 0);
    ~ObjectInspectorWidget();

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
    QString id() const Q_DECL_OVERRIDE { return QStringLiteral("GammaRay::ObjectInspector"); }
    QString name() const Q_DECL_OVERRIDE { return tr("Objects"); }
    QWidget *createWidget(QWidget *parentWidget) Q_DECL_OVERRIDE
    {
        return new ObjectInspectorWidget(
            parentWidget);
    }
    void initUi() Q_DECL_OVERRIDE
    {
        PropertyWidget::registerTab<PropertiesTab>(QStringLiteral("properties"), tr(
                                                       "Properties"),
                                                   PropertyWidgetTabPriority::First);
        ObjectBroker::registerClientObjectFactoryCallback<PropertiesExtensionInterface *>(
            createExtension<PropertiesExtensionClient>);
        PropertyWidget::registerTab<MethodsTab>(QStringLiteral("methods"), tr(
                                                    "Methods"),
                                                PropertyWidgetTabPriority::Basic - 1);
        ObjectBroker::registerClientObjectFactoryCallback<MethodsExtensionInterface *>(
            createExtension<MethodsExtensionClient>);
        PropertyWidget::registerTab<ConnectionsTab>(QStringLiteral("connections"), tr(
                                                        "Connections"),
                                                    PropertyWidgetTabPriority::Basic - 1);
        ObjectBroker::registerClientObjectFactoryCallback<ConnectionsExtensionInterface *>(
            createExtension<ConnectionsExtensionClient>);
        PropertyWidget::registerTab<EnumsTab>(QStringLiteral("enums"), tr(
                                                  "Enums"), PropertyWidgetTabPriority::Exotic - 1);
        PropertyWidget::registerTab<ClassInfoTab>(QStringLiteral("classInfo"), tr(
                                                      "Class Info"),
                                                  PropertyWidgetTabPriority::Exotic - 1);
        PropertyWidget::registerTab<ApplicationAttributeTab>(QStringLiteral(
                                                                 "applicationAttributes"),
                                                             tr(
                                                                 "Attributes"),
            PropertyWidgetTabPriority::Advanced);
    }
};
}

#endif // GAMMARAY_OBJECTINSPECTOR_H
