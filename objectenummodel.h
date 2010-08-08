#ifndef ENDOSCOPE_OBJECTENUMMODEL_H
#define ENDOSCOPE_OBJECTENUMMODEL_H

#include "metaobjectmodel.h"

namespace Endoscope {

class ObjectEnumModel : public MetaObjectModel<QMetaEnum, &QMetaObject::enumerator, &QMetaObject::enumeratorCount, &QMetaObject::enumeratorOffset>
{
  public:
    ObjectEnumModel(QObject* parent = 0);
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QString columnHeader(int index) const;
    QVariant data(const QModelIndex& index, const QMetaEnum& enumerator, int role) const;
};

}

#endif // ENDOSCOPE_OBJECTENUMMODEL_H
