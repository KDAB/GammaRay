#ifndef ENDOSCOPE_OBJECTSTATICPROPERTYMODEL_H
#define ENDOSCOPE_OBJECTSTATICPROPERTYMODEL_H

#include <objectpropertymodel.h>


namespace Endoscope {

class ObjectStaticPropertyModel : public ObjectPropertyModel
{
  Q_OBJECT
  public:
    explicit ObjectStaticPropertyModel(QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
};

}

#endif // ENDOSCOPE_OBJECTSTATICPROPERTYMODEL_H
