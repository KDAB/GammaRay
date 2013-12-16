/*
  propertycontroller.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertycontroller.h"

#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"
#include "metapropertymodel.h"
#include "methodargumentmodel.h"
#include "multisignalmapper.h"
#include "objectclassinfomodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectenummodel.h"
#include "objectmethodmodel.h"
#include "objectstaticpropertymodel.h"
#include "probe.h"

#include "remote/remotemodelserver.h"

#include "common/objectbroker.h"
#include "common/enums.h"

#include <QDebug>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QTime>

using namespace GammaRay;

PropertyController::PropertyController(const QString &baseName, QObject *parent) :
  PropertyControllerInterface(baseName + ".controller", parent),
  m_objectBaseName(baseName),
  m_staticPropertyModel(new ObjectStaticPropertyModel(this)),
  m_dynamicPropertyModel(new ObjectDynamicPropertyModel(this)),
  m_classInfoModel(new ObjectClassInfoModel(this)),
  m_methodModel(new ObjectMethodModel(this)),
  m_inboundConnectionModel(new ConnectionFilterProxyModel(this)),
  m_outboundConnectionModel(new ConnectionFilterProxyModel(this)),
  m_enumModel(new ObjectEnumModel(this)),
  m_signalMapper(0),
  m_methodLogModel(new QStandardItemModel(this)),
  m_metaPropertyModel(new MetaPropertyModel(this)),
  m_methodArgumentModel(new MethodArgumentModel(this))
{
  m_inboundConnectionModel->setFilterOnReceiver(true);
  m_outboundConnectionModel->setFilterOnSender(true);

  registerModel(m_staticPropertyModel, "staticProperties");
  registerModel(m_dynamicPropertyModel, "dynamicProperties");
  registerModel(m_methodModel, "methods");
  registerModel(m_methodLogModel, "methodLog");
  registerModel(m_classInfoModel, "classInfo");
  registerModel(m_inboundConnectionModel, "inboundConnections");
  registerModel(m_outboundConnectionModel, "outboundConnections");
  registerModel(m_enumModel, "enums");
  registerModel(m_metaPropertyModel, "nonQProperties");
  registerModel(m_methodArgumentModel, "methodArguments");

  ObjectBroker::selectionModel(m_methodModel); // trigger creation

  // TODO 1.3 had an optimization using the ProxyDetacher for these models, re-add that
  m_inboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());
  m_outboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());
}

PropertyController::~PropertyController()
{
}

void PropertyController::registerModel(QAbstractItemModel *model, const QString &nameSuffix)
{
  Probe::instance()->registerModel(m_objectBaseName + '.' + nameSuffix, model);
}

void PropertyController::signalEmitted(QObject *sender, int signalIndex)
{
  Q_ASSERT(m_object == sender);
  m_methodLogModel->appendRow(
  new QStandardItem(tr("%1: Signal %2 emitted").
  arg(QTime::currentTime().toString("HH:mm:ss.zzz")).
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  arg(sender->metaObject()->method(signalIndex).signature())));
#else
  arg(QString(sender->metaObject()->method(signalIndex).methodSignature()))));
#endif
}

void PropertyController::setObject(QObject *object)
{
  m_object = object;
  m_staticPropertyModel->setObject(object);
  m_dynamicPropertyModel->setObject(object);
  m_inboundConnectionModel->filterReceiver(object);
  m_outboundConnectionModel->filterSender(object);

  const QMetaObject *metaObject = 0;
  if (object) {
    metaObject = object->metaObject();
  }
  m_enumModel->setMetaObject(metaObject);
  m_classInfoModel->setMetaObject(metaObject);
  m_methodModel->setMetaObject(metaObject);

  delete m_signalMapper;
  m_signalMapper = new MultiSignalMapper(this);
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int)), SLOT(signalEmitted(QObject*,int)));

  m_methodLogModel->clear();

  m_metaPropertyModel->setObject(object);

  emit displayStateChanged(PropertyWidgetDisplayState::QObject);
}

void PropertyController::setObject(void *object, const QString &className)
{
  setObject(0);
  m_metaPropertyModel->setObject(object, className);

  emit displayStateChanged(PropertyWidgetDisplayState::Object);
}

void PropertyController::setMetaObject(const QMetaObject *metaObject)
{
  setObject(0);
  m_enumModel->setMetaObject(metaObject);
  m_classInfoModel->setMetaObject(metaObject);
  m_methodModel->setMetaObject(metaObject);

  emit displayStateChanged(PropertyWidgetDisplayState::MetaObject);
}

void PropertyController::activateMethod()
{
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(m_methodModel);
  if (selectionModel->selectedRows().size() != 1) {
    return;
  }
  const QModelIndex index = selectionModel->selectedRows().first();

  const QMetaMethod method = index.data(ObjectMethodModelRole::MetaMethod).value<QMetaMethod>();
  if (method.methodType() == QMetaMethod::Slot) {
    m_methodArgumentModel->setMethod(method);
  } else if (method.methodType() == QMetaMethod::Signal) {
    m_signalMapper->connectToSignal(m_object, method);
  }
}

void PropertyController::invokeMethod(Qt::ConnectionType connectionType)
{
  if (!m_object) {
    m_methodLogModel->appendRow(
      new QStandardItem(
        tr("%1: Invocation failed: Invalid object, probably got deleted in the meantime.").
        arg(QTime::currentTime().toString("HH:mm:ss.zzz"))));
    return;
  }

  QMetaMethod method;
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(m_methodModel);
  if (selectionModel->selectedRows().size() == 1) {
    const QModelIndex index = selectionModel->selectedRows().first();
    method = index.data(ObjectMethodModelRole::MetaMethod).value<QMetaMethod>();
  }

  if (method.methodType() != QMetaMethod::Slot) {
    m_methodLogModel->appendRow(
      new QStandardItem(
        tr("%1: Invocation failed: Invalid method (not a slot?).").
        arg(QTime::currentTime().toString("HH:mm:ss.zzz"))));
    return;
  }

  const QVector<MethodArgument> args = m_methodArgumentModel->arguments();
  // TODO retrieve return value and add it to the log in case of success
  // TODO measure executation time and that to the log
  const bool result = method.invoke(m_object.data(), connectionType,
    args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);

  if (!result) {
    m_methodLogModel->appendRow(
      new QStandardItem(
        tr("%1: Invocation failed..").
        arg(QTime::currentTime().toString("HH:mm:ss.zzz"))));
    return;
  }

  m_methodArgumentModel->setMethod(QMetaMethod());
}

