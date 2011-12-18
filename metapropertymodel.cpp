#include "metapropertymodel.h"
#include "metaobjectrepository.h"
#include "metaobject.h"
#include "util.h"

using namespace GammaRay;

MetaPropertyModel::MetaPropertyModel(QObject* parent):
  QAbstractTableModel(parent),
  m_metaObject(0),
  m_object(0)
{
}

void MetaPropertyModel::setObject(void* object, const QString& typeName)
{
  beginResetModel();
  m_object = object;
  m_metaObject = MetaObjectRepository::instance()->metaObject(typeName);
  endResetModel();
}

void MetaPropertyModel::setObject(QObject *object)
{
  beginResetModel();
  m_object = 0;
    m_metaObject = 0;

  if (object) {
    const QMetaObject *mo = object->metaObject();
    while (mo && !m_metaObject) {
      m_metaObject = MetaObjectRepository::instance()->metaObject(mo->className());
      mo = mo->superClass();
    }
    if (m_metaObject)
      m_object = object;
  }
  endResetModel();
}

int MetaPropertyModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 4;
}

int MetaPropertyModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid() || !m_metaObject)
    return 0;
  return m_metaObject->propertyCount();
}

QVariant MetaPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Property");
    case 1:
      return tr("Value");
    case 2:
      return tr("Type");
    case 3:
      return tr("Class");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant MetaPropertyModel::data(const QModelIndex& index, int role) const
{
  if (!m_metaObject || !index.isValid())
    return QVariant();

  MetaProperty *property = m_metaObject->propertyAt(index.row());
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0: return property->name();
      case 2: return property->typeName();
      case 3: return property->metaObject()->className();
    }
  }

  if (index.column() == 1) {
    if (!m_object)
      return QVariant();
    const QVariant value = property->value(m_object); // TODO: cache this, to make this more robust against m_object becoming invalid
    switch (role) {
      case Qt::DisplayRole:
        return Util::variantToString(value);
      case Qt::EditRole:
        return value;
    }
  }
  return QVariant();
}

bool MetaPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && index.column() == 1 && m_metaObject && m_object && role == Qt::EditRole) {
    MetaProperty *property = m_metaObject->propertyAt(index.row());
    property->setValue(m_object, value);
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags MetaPropertyModel::flags(const QModelIndex& index) const
{
  const Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!index.isValid() || index.column() != 1 || !m_metaObject || !m_object)
    return f;

  MetaProperty *property = m_metaObject->propertyAt(index.row());
  if (property->isReadOnly())
    return f;
  return f | Qt::ItemIsEditable;
}

#include "metapropertymodel.moc"
