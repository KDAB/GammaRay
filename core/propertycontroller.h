/*
  propertycontroller.h

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

#ifndef GAMMARAY_PROPERTYCONTROLLER_H
#define GAMMARAY_PROPERTYCONTROLLER_H

#include "gammaray_core_export.h"

#include <common/propertycontrollerinterface.h>

#include <QPointer>

class QAbstractItemModel;
class QStandardItemModel;

namespace GammaRay {

class ConnectionFilterProxyModel;
class MultiSignalMapper;
class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class ObjectClassInfoModel;
class ObjectMethodModel;
class ObjectEnumModel;
class MetaPropertyModel;
class MethodArgumentModel;

/** Non-UI part of the property widget. */
class GAMMARAY_CORE_EXPORT PropertyController : public PropertyControllerInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::PropertyControllerInterface)
public:
  explicit PropertyController(const QString &baseName, QObject *parent);
  ~PropertyController();

  void setObject(QObject *object);
  void setObject(void *object, const QString &className);
  void setMetaObject(const QMetaObject *metaObject);

public slots:
  void activateMethod();
  void invokeMethod(Qt::ConnectionType type);

private slots:
  void signalEmitted(QObject *sender, int signalIndex);

private:
  void registerModel(QAbstractItemModel *model, const QString &nameSuffix);

private:
  QString m_objectBaseName;

  QPointer<QObject> m_object;
  ObjectStaticPropertyModel *m_staticPropertyModel;
  ObjectDynamicPropertyModel *m_dynamicPropertyModel;
  ObjectClassInfoModel *m_classInfoModel;
  ObjectMethodModel *m_methodModel;
  ConnectionFilterProxyModel *m_inboundConnectionModel;
  ConnectionFilterProxyModel *m_outboundConnectionModel;
  ObjectEnumModel *m_enumModel;
  MultiSignalMapper *m_signalMapper;
  QStandardItemModel *m_methodLogModel;
  MetaPropertyModel *m_metaPropertyModel;

  MethodArgumentModel *m_methodArgumentModel;
};

}

#endif // GAMMARAY_PROPERTYCONTROLLER_H
