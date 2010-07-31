#ifndef ENDOSCOPE_OBJECTDYNAMICPROPERTYMODEL_H
#define ENDOSCOPE_OBJECTDYNAMICPROPERTYMODEL_H

#include "objectpropertymodel.h"


namespace Endoscope {

class ObjectDynamicPropertyModel : public ObjectPropertyModel
{
  Q_OBJECT
  public:
    explicit ObjectDynamicPropertyModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
};

}

#endif // ENDOSCOPE_OBJECTDYNAMICPROPERTYMODEL_H
