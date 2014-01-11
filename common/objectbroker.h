/*
  objectbroker.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTBROKER_H
#define GAMMARAY_OBJECTBROKER_H

#include "gammaray_common_export.h"

#include <QObject>

class QItemSelectionModel;
class QAbstractItemModel;

namespace GammaRay {

/** @brief Retrieve/expose objects independent of whether using in-process or out-of-process UI. */
namespace ObjectBroker {

  /** Register a newly created QObject under the given name. */
  GAMMARAY_COMMON_EXPORT void registerObject(const QString &name, QObject *object);
  template<class T>
  void registerObject(QObject *object)
  {
    const QString interface = QString::fromUtf8(qobject_interface_iid<T>());
    registerObject(interface, object);
  }

  /** Retrieve object by name. */
  GAMMARAY_COMMON_EXPORT QObject* objectInternal(const QString &name, const QByteArray &type = QByteArray());

  /**
   * Retrieve an object by name implementing interface @p T.
   *
   * Use this if multiple objects of the given type have been registered.
   * Otherwise the function below is simpler and more failsafe.
   *
   * NOTE: the "T = 0" is just to ensure a pointer type is given.
   */
  template<class T>
  T object(const QString &name, T = 0)
  {
    T ret = qobject_cast<T>(objectInternal(name, QByteArray(qobject_interface_iid<T>())));
    Q_ASSERT(ret);
    return ret;
  }

  /**
   * Retrieve object implementing interface @p T.
   *
   * This only works if a single type was registered implementing this interface
   * using qobject_interface_iid as object name.
   *
   * In most cases this is the simplest way for tools to get an object.
   *
   * NOTE: the "T = 0" is just to ensure a pointer type is given.
   */
  template<class T>
  T object(T = 0)
  {
    const QByteArray interface(qobject_interface_iid<T>());
    T ret = qobject_cast<T>(objectInternal(QString::fromUtf8(interface), interface));
    Q_ASSERT(ret);
    return ret;
  }

  typedef QObject*(*ClientObjectFactoryCallback)(const QString &, QObject *parent);

  /** Register a callback for a factory to create remote object stubs for the given type. */
  GAMMARAY_COMMON_EXPORT void registerClientObjectFactoryCallbackInternal(const QByteArray &type, ClientObjectFactoryCallback callback);

  /**
   * Register a callback for a factory of a given interface to create remote object stubs for the given type.
   *
   * NOTE: the "T = 0" is just to ensure a pointer type is given.
   */
  template<class T>
  void registerClientObjectFactoryCallback(ClientObjectFactoryCallback callback, T = 0)
  {
    registerClientObjectFactoryCallbackInternal(QByteArray(qobject_interface_iid<T>()), callback);
  }

  /**
   * Register a newly created model with the given name.
   *
   * NOTE: This must not be called directly by anything but the probe/server side.
   * User code must use Probe::registerModel() instead!
   */
  GAMMARAY_COMMON_EXPORT void registerModelInternal(const QString &name, QAbstractItemModel* model);

  /** Retrieve a model by name. */
  GAMMARAY_COMMON_EXPORT QAbstractItemModel* model(const QString &name);

  typedef QAbstractItemModel*(*ModelFactoryCallback)(const QString &);

  /** Set a callback for the case that a model was requested but had not been registered before. */
  GAMMARAY_COMMON_EXPORT void setModelFactoryCallback(ModelFactoryCallback callback);

  /** Register a newly created selection model. */
  GAMMARAY_COMMON_EXPORT void registerSelectionModel(QItemSelectionModel *selectionModel);
  /** Unregisters a selection model. You have to take care of deletion yourself. */
  GAMMARAY_COMMON_EXPORT void unregisterSelectionModel(QItemSelectionModel *selectionModel);
  /** Checks whether a selection model for the given @p model is registered already. */
  GAMMARAY_COMMON_EXPORT bool hasSelectionModel(QAbstractItemModel* model);

  /** Retrieve the selection model for @p model. */
  GAMMARAY_COMMON_EXPORT QItemSelectionModel* selectionModel(QAbstractItemModel *model);

  typedef QItemSelectionModel*(*selectionModelFactoryCallback)(QAbstractItemModel*);

  /** Set a callback for the case that a selection model was requested but had not been registered before. */
  GAMMARAY_COMMON_EXPORT void setSelectionModelFactoryCallback(selectionModelFactoryCallback callback);

  /** Clear all registered objects. Useful when the probe is deleted. */
  GAMMARAY_COMMON_EXPORT void clear();
}
}

#endif // GAMMARAY_OBJECTBROKER_H
