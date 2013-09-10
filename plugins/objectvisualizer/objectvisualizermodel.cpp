#include "objectvisualizermodel.h"

#include <include/util.h>

using namespace GammaRay;

ObjectVisualizerModel::ObjectVisualizerModel(QObject* parent) : QIdentityProxyModel(parent)
{
}

ObjectVisualizerModel::~ObjectVisualizerModel()
{
}

QVariant ObjectVisualizerModel::data(const QModelIndex& proxyIndex, int role) const
{
  if (role == ObjectDisplayName) {
    QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject*>();
    return Util::displayString(obj);
  }
  else if (role == ObjectId) {
    QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject*>();
    return static_cast<qulonglong>(reinterpret_cast<quintptr>(obj));
  }
  else if (role == ClassName) {
    QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject*>();
    Q_ASSERT(obj);
    return obj->metaObject()->className();
  }

  return QIdentityProxyModel::data(proxyIndex, role);
}

QMap< int, QVariant > ObjectVisualizerModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> d = QIdentityProxyModel::itemData(index);
  d.insert(ObjectId, data(index, ObjectId));
  d.insert(ObjectDisplayName, data(index, ObjectDisplayName));
  d.insert(ClassName, data(index, ClassName));
  return d;
}

#include "objectvisualizermodel.moc"
