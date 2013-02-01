#include "modelbroker.h"

#include <QHash>
#include <QString>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace GammaRay {

struct ModelBrokerData {
  ModelBrokerData() : modelCallback(0), selectionCallback(0) {}
  QHash<QString, QAbstractItemModel*> models;
  QHash<QAbstractItemModel*, QItemSelectionModel*> selectionModels;
  ModelBroker::ModelNotFoundCallback modelCallback;
  ModelBroker::selectionModelNotFoundCallback selectionCallback;
};

Q_GLOBAL_STATIC(ModelBrokerData, s_modelBroker)

void ModelBroker::registerModel(const QString& name, QAbstractItemModel* model)
{
  Q_ASSERT(!s_modelBroker()->models.contains(name));
  model->setObjectName(name);
  s_modelBroker()->models.insert(name, model);
}

QAbstractItemModel* ModelBroker::model(const QString& name)
{
  const QHash<QString, QAbstractItemModel*>::const_iterator it = s_modelBroker()->models.constFind(name);
  if (it != s_modelBroker()->models.constEnd())
    return it.value();

  if (s_modelBroker()->modelCallback) {
    QAbstractItemModel* model = s_modelBroker()->modelCallback(name);
    if (model) {
      model->setObjectName(name);
      s_modelBroker()->models.insert(name, model);
      return model;
    }
  }
  return 0;
}

void ModelBroker::setModelNotFoundCallback(ModelBroker::ModelNotFoundCallback callback)
{
  s_modelBroker()->modelCallback = callback;
}

void ModelBroker::registerSelectionModel(QItemSelectionModel* selectionModel)
{
  Q_ASSERT(!s_modelBroker()->selectionModels.contains(const_cast<QAbstractItemModel*>(selectionModel->model())));
  s_modelBroker()->selectionModels.insert(const_cast<QAbstractItemModel*>(selectionModel->model()), selectionModel);
}

QItemSelectionModel* ModelBroker::selectionModel(QAbstractItemModel* model)
{
  const QHash<QAbstractItemModel*, QItemSelectionModel*>::const_iterator it = s_modelBroker()->selectionModels.constFind(model);
  if (it != s_modelBroker()->selectionModels.constEnd())
    return it.value();

  if (s_modelBroker()->selectionCallback) {
    QItemSelectionModel* selectionModel = s_modelBroker()->selectionCallback(model);
    if (selectionModel) {
      s_modelBroker()->selectionModels.insert(model, selectionModel);
      return selectionModel;
    }
  }
  return 0;
}

void ModelBroker::setSelectionModelNotFoundCallback(ModelBroker::selectionModelNotFoundCallback callback)
{
  s_modelBroker()->selectionCallback = callback;
}

}
