#include "modelbroker.h"

#include <QHash>
#include <QString>

namespace GammaRay {

struct ModelBrokerData {
  ModelBrokerData() : callback(0) {}
  QHash<QString, QAbstractItemModel*> models;
  ModelBroker::ModelNotFoundCallback callback;
};

Q_GLOBAL_STATIC(ModelBrokerData, s_modelBroker)

void ModelBroker::registerModel(const QString& name, QAbstractItemModel* model)
{
  Q_ASSERT(!s_modelBroker()->models.contains(name));
  s_modelBroker()->models.insert(name, model);
}

QAbstractItemModel* ModelBroker::model(const QString& name)
{
  const QHash<QString, QAbstractItemModel*>::const_iterator it = s_modelBroker()->models.constFind(name);
  if (it != s_modelBroker()->models.constEnd())
    return it.value();

  if (s_modelBroker()->callback) {
    QAbstractItemModel* model = s_modelBroker()->callback(name);
    if (model) {
      s_modelBroker()->models.insert(name, model);
      return model;
    }
  }
  return 0;
}

void ModelBroker::setModelNotFoundCallback(ModelBroker::ModelNotFoundCallback callback)
{
  s_modelBroker()->callback = callback;
}

}
