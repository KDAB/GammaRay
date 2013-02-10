#include "objectbroker.h"
#include "networkobject.h"

#include <kde/klinkitemselectionmodel.h>

#include <QHash>
#include <QString>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QAbstractProxyModel>

namespace GammaRay {

struct ObjectlBrokerData {
  ObjectlBrokerData() : objectCallback(0), modelCallback(0), selectionCallback(0) {}
  QHash<QString, NetworkObject*> objects;
  QHash<QString, QAbstractItemModel*> models;
  QHash<QAbstractItemModel*, QItemSelectionModel*> selectionModels;
  ObjectBroker::ObjectFactoryCallback objectCallback;
  ObjectBroker::ModelFactoryCallback modelCallback;
  ObjectBroker::selectionModelFactoryCallback selectionCallback;
};

Q_GLOBAL_STATIC(ObjectlBrokerData, s_objectBroker)

void ObjectBroker::registerObject(const QString& name, NetworkObject* object)
{
  Q_ASSERT(!s_objectBroker()->objects.contains(name));
  object->setObjectName(name);
  s_objectBroker()->objects.insert(name, object);
}

NetworkObject* ObjectBroker::object(const QString& name)
{
  const QHash<QString, NetworkObject*>::const_iterator it = s_objectBroker()->objects.constFind(name);
  if (it != s_objectBroker()->objects.constEnd())
    return it.value();

  if (s_objectBroker()->objectCallback) {
    NetworkObject* obj = s_objectBroker()->objectCallback(name);
    if (obj) {
      registerObject(name, obj);
      return obj;
    }
  }
  return 0;
}

void ObjectBroker::setObjectFactoryCallback(ObjectBroker::ObjectFactoryCallback callback)
{
  s_objectBroker()->objectCallback = callback;
}

void ObjectBroker::registerModel(const QString& name, QAbstractItemModel* model)
{
  Q_ASSERT(!s_objectBroker()->models.contains(name));
  model->setObjectName(name);
  s_objectBroker()->models.insert(name, model);
}

QAbstractItemModel* ObjectBroker::model(const QString& name)
{
  const QHash<QString, QAbstractItemModel*>::const_iterator it = s_objectBroker()->models.constFind(name);
  if (it != s_objectBroker()->models.constEnd())
    return it.value();

  if (s_objectBroker()->modelCallback) {
    QAbstractItemModel* model = s_objectBroker()->modelCallback(name);
    if (model) {
      model->setObjectName(name);
      s_objectBroker()->models.insert(name, model);
      return model;
    }
  }
  return 0;
}

void ObjectBroker::setModelFactoryCallback(ObjectBroker::ModelFactoryCallback callback)
{
  s_objectBroker()->modelCallback = callback;
}

void ObjectBroker::registerSelectionModel(QItemSelectionModel* selectionModel)
{
  Q_ASSERT(!s_objectBroker()->selectionModels.contains(const_cast<QAbstractItemModel*>(selectionModel->model())));
  s_objectBroker()->selectionModels.insert(const_cast<QAbstractItemModel*>(selectionModel->model()), selectionModel);
}

static QAbstractItemModel* sourceModelForProxy(QAbstractItemModel* model)
{
  QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel*>(model);
  if (!proxy)
    return model;
  return sourceModelForProxy(proxy->sourceModel());
}

QItemSelectionModel* ObjectBroker::selectionModel(QAbstractItemModel* model)
{
  const QHash<QAbstractItemModel*, QItemSelectionModel*>::const_iterator it = s_objectBroker()->selectionModels.constFind(model);
  if (it != s_objectBroker()->selectionModels.constEnd())
    return it.value();

  if (s_objectBroker()->selectionCallback) {
    QAbstractItemModel *sourceModel = sourceModelForProxy(model);

    QItemSelectionModel* selectionModel = 0;
    if (sourceModel == model) {
      selectionModel = s_objectBroker()->selectionCallback(sourceModel);
    } else {
      QItemSelectionModel *sourceSelectionModel = ObjectBroker::selectionModel(sourceModel);
      selectionModel = new KLinkItemSelectionModel(model, sourceSelectionModel, model);
    }

    if (selectionModel) {
      s_objectBroker()->selectionModels.insert(model, selectionModel);
      return selectionModel;
    }
  }
  return 0;
}

void ObjectBroker::setSelectionModelFactoryCallback(ObjectBroker::selectionModelFactoryCallback callback)
{
  s_objectBroker()->selectionCallback = callback;
}

}
