#ifndef ENDOSCOPE_OBJECTENUMMODEL_H
#define ENDOSCOPE_OBJECTENUMMODEL_H

#include "metaobjectmodel.h"

namespace Endoscope {

class ObjectEnumModel : public MetaObjectModel<QMetaEnum, &QMetaObject::enumerator, &QMetaObject::enumeratorCount, &QMetaObject::enumeratorOffset>
{
  public:
    ObjectEnumModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QString columnHeader(int index) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex& index, const QMetaEnum& enumerator, int role) const;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
};

}

#endif // ENDOSCOPE_OBJECTENUMMODEL_H
