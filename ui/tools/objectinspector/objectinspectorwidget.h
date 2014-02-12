/*
  objectinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include "inboundconnectionstab.h"
#include "outboundconnectionstab.h"
#include "enumstab.h"
#include "classinfotab.h"

class QItemSelection;

namespace GammaRay {

namespace Ui {
  class ObjectInspectorWidget;
}

class ObjectInspectorWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit ObjectInspectorWidget(QWidget *parent = 0);
    ~ObjectInspectorWidget();

  private slots:
    void objectSelectionChanged(const QItemSelection &selection);

  private:
    QScopedPointer<Ui::ObjectInspectorWidget> ui;
};

class ObjectInspectorFactory : public ToolUiFactory {
public:
  virtual inline QString id() const { return "GammaRay::ObjectInspector"; }
  virtual inline QWidget *createWidget(QWidget *parentWidget) { return new ObjectInspectorWidget(parentWidget); }
  virtual inline bool remotingSupported() const { return true; }
  virtual void initUi()
  {
    PropertyWidget::registerTab<PropertiesTab>(QObject::tr("Properties"));
    PropertyWidget::registerTab<MethodsTab>(QObject::tr("Methods"));
    PropertyWidget::registerTab<InboundConnectionsTab>(QObject::tr("Inbound Connections"));
    PropertyWidget::registerTab<OutboundConnectionsTab>(QObject::tr("Outbound Connections"));
    PropertyWidget::registerTab<EnumsTab>(QObject::tr("Enums"));
    PropertyWidget::registerTab<ClassInfoTab>(QObject::tr("Class Info"));
  }
};

}

#endif // GAMMARAY_OBJECTINSPECTOR_H
