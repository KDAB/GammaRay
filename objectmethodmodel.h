#ifndef ENDOSCOPE_OBJECTMETHODMODEL_H
#define ENDOSCOPE_OBJECTMETHODMODEL_H

#include "metaobjectmodel.h"

#include <QMetaMethod>

namespace Endoscope {

class ObjectMethodModel : public MetaObjectModel<QMetaMethod, &QMetaObject::method, &QMetaObject::methodCount, &QMetaObject::methodOffset>
{
  public:
    ObjectMethodModel(QObject* parent = 0);
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

  protected:
    QVariant data(const QModelIndex& index, const QMetaMethod &method, int role = Qt::DisplayRole) const;
    QString columnHeader(int index) const;
};

}

#endif // ENDOSCOPE_OBJECTMETHODMODEL_H
